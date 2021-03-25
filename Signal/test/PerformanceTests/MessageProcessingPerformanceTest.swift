//
//  Copyright (c) 2021 Open Whisper Systems. All rights reserved.
//

import XCTest
@testable import SignalServiceKit
import GRDB

class MessageProcessingPerformanceTest: PerformanceBaseTest {

    let localE164Identifier = "+13235551234"
    let localUUID = UUID()
    let localClient = LocalSignalClient()

    let bobUUID = UUID()
    var bobClient: TestSignalClient!

    let runner = TestProtocolRunner()
    lazy var fakeService = FakeService(localClient: localClient, runner: runner)

    var dbObserverBlock: (() -> Void)?

    private var dbObserver: BlockObserver?

    // MARK: - Hooks

    override func setUp() {
        super.setUp()

        storageCoordinator.useGRDBForTests()
        try! databaseStorage.grdbStorage.setupUIDatabase()

        let dbObserver = BlockObserver(block: { [weak self] in self?.dbObserverBlock?() })
        self.dbObserver = dbObserver
        databaseStorage.appendUIDatabaseSnapshotDelegate(dbObserver)
    }

    override func tearDown() {
        super.tearDown()

        self.dbObserver = nil
        databaseStorage.grdbStorage.testing_tearDownUIDatabase()
    }

    // MARK: - Tests

    func testGRDBPerf_messageProcessing() {
        measureMetrics(XCTestCase.defaultPerformanceMetrics, automaticallyStartMeasuring: false) {
            processIncomingMessages()
        }
    }

    func processIncomingMessages() {
        // ensure local client has necessary "registered" state
        identityManager.generateNewIdentityKey()
        tsAccountManager.registerForTests(withLocalNumber: localE164Identifier, uuid: localUUID)

        // use the uiDatabase to be notified of DB writes so we can verify the expected
        // changes occur
        bobClient = FakeSignalClient.generate(uuid: bobUUID)

        write { transaction in
            XCTAssertEqual(0, TSMessage.anyCount(transaction: transaction))
            XCTAssertEqual(0, TSThread.anyCount(transaction: transaction))

            try! self.runner.initialize(senderClient: self.bobClient,
                                        recipientClient: self.localClient,
                                        transaction: transaction)
        }

        let buildEnvelopeData = { () -> Data in
            let envelopeBuilder = try! self.fakeService.envelopeBuilder(fromSenderClient: self.bobClient)
            envelopeBuilder.setSourceUuid(self.bobUUID.uuidString)
            return try! envelopeBuilder.buildSerializedData()
        }

        let envelopeCount: Int = DebugFlags.fastPerfTests ? 5 : 500
        let envelopeDatas: [Data] = (0..<envelopeCount).map { _ in buildEnvelopeData() }

        // Wait until message processing has completed, otherwise future
        // tests may break as we try and drain the processing queue.
        let expectFlushNotification = expectation(description: "queue flushed")
        NotificationCenter.default.observe(once: MessageProcessor.messageProcessorDidFlushQueue).done { _ in
            expectFlushNotification.fulfill()
        }

        let expectMessagesProcessed = expectation(description: "messages processed")
        let hasFulfilled = AtomicBool(false)
        let fulfillOnce = {
            if hasFulfilled.tryToSetFlag() {
                expectMessagesProcessed.fulfill()
            }
        }

        self.dbObserverBlock = {
            let messageCount = self.databaseStorage.read { transaction in
                return TSInteraction.anyCount(transaction: transaction)
            }
            if messageCount == envelopeDatas.count {
                fulfillOnce()
            }
        }

        startMeasuring()

        messageProcessor.processEncryptedEnvelopes(
            envelopes: envelopeDatas.map { ($0, nil, { XCTAssertNil($0) }) },
            serverDeliveryTimestamp: 0
        )

        waitForExpectations(timeout: 15.0) { _ in
            self.stopMeasuring()

            self.dbObserverBlock = nil
            self.write { transaction in
                TSInteraction.anyRemoveAllWithInstantation(transaction: transaction)
                TSThread.anyRemoveAllWithInstantation(transaction: transaction)
                SSKMessageDecryptJobRecord.anyRemoveAllWithInstantation(transaction: transaction)
                OWSMessageContentJob.anyRemoveAllWithInstantation(transaction: transaction)
                OWSRecipientIdentity.anyRemoveAllWithInstantation(transaction: transaction)
            }
        }
    }
}

private class BlockObserver: UIDatabaseSnapshotDelegate {
    let block: () -> Void
    init(block: @escaping () -> Void) {
        self.block = block
    }

    func uiDatabaseSnapshotWillUpdate() {
        AssertIsOnMainThread()
    }

    func uiDatabaseSnapshotDidUpdate(databaseChanges: UIDatabaseChanges) {
        block()
    }

    func uiDatabaseSnapshotDidUpdateExternally() {
        block()
    }

    func uiDatabaseSnapshotDidReset() {
        block()
    }
}
