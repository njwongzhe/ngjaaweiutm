#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

using Date = string;
using Decimal = double;

// =====================================================
// Enumerations
// =====================================================

enum class TransactionStatus {
    NORMAL,
    DIBATALKAN,
    DILARASKAN,
    PEMBALIKAN,
    PELARASAN
};

enum class TransactionCategory {
    BAYARAN,
    CAJ_SEWA,
    CAJ_PENYELENGGARAAN,
    CAJ_PENALTI,
    CAJ_TAMBAH,
    REBAT,
    BAKI_AWAL,
    LAIN_LAIN
};

string statusToString(TransactionStatus status) {
    switch (status) {
        case TransactionStatus::NORMAL:
            return "NORMAL";
        case TransactionStatus::DIBATALKAN:
            return "DIBATALKAN";
        case TransactionStatus::DILARASKAN:
            return "DILARASKAN";
        case TransactionStatus::PEMBALIKAN:
            return "PEMBALIKAN";
        case TransactionStatus::PELARASAN:
            return "PELARASAN";
        default:
            return "UNKNOWN";
    }
}

string categoryToString(TransactionCategory category) {
    switch (category) {
        case TransactionCategory::BAYARAN:
            return "BAYARAN";
        case TransactionCategory::CAJ_SEWA:
            return "CAJ_SEWA";
        case TransactionCategory::CAJ_PENYELENGGARAAN:
            return "CAJ_PENYELENGGARAAN";
        case TransactionCategory::CAJ_PENALTI:
            return "CAJ_PENALTI";
        case TransactionCategory::CAJ_TAMBAH:
            return "CAJ_TAMBAH";
        case TransactionCategory::REBAT:
            return "REBAT";
        case TransactionCategory::BAKI_AWAL:
            return "BAKI_AWAL";
        case TransactionCategory::LAIN_LAIN:
            return "LAIN_LAIN";
        default:
            return "UNKNOWN";
    }
}

// =====================================================
// Basic Models, DTOs and Request Models
// =====================================================

struct AdminProfile {
    string id;
    string username;
};

struct QueryFilter {
    string search;
    Date startDate;
    Date endDate;
};

struct TransactionFilterParams {
    string search;
    Date startDate;
    Date endDate;
    int page;
    int limit;

    QueryFilter toQuery() const {
        return { search, startDate, endDate };
    }

    bool hasActiveFilters() const {
        return !search.empty() || !startDate.empty() || !endDate.empty();
    }
};

template <typename T>
struct PaginatedResult {
    vector<T> items;
    int page;
    int limit;
    int total;
};

template <typename T>
struct ApiResponse {
    bool success;
    string message;
    T data;
};

struct TransactionListRequest {
    TransactionFilterParams params;
};

struct TransactionAdjustRequest {
    string transactionId;
    Decimal newAmount;
    string remarks;
    AdminProfile actor;
};

struct TransactionReverseRequest {
    string transactionId;
    string remarks;
    AdminProfile actor;
};

struct TransactionDto {
    string id;
    string transactionNo;
    string residentId;
    string paymentId;
    Date transactionDate;
    string category;
    string status;
    Decimal debitAmount;
    Decimal creditAmount;
    Decimal netAmount;
    string relatedTransactionId;
};

struct TransactionSummary {
    int totalCount;
    Decimal totalDebit;
    Decimal totalCredit;

    Decimal getNetTotal() const {
        return totalDebit - totalCredit;
    }
};

struct CommandResult;

// =====================================================
// Domain Entity: Resident
// =====================================================

class Resident {
private:
    string id;
    string fullName;
    string icNumber;
    string phone;
    string email;
    string position;
    string department;
    string serviceLevel;
    string status;

public:
    Resident() {
        id = "";
        fullName = "";
        icNumber = "";
        phone = "";
        email = "";
        position = "";
        department = "";
        serviceLevel = "";
        status = "AKTIF";
    }

    Resident(string id, string fullName, string icNumber) {
        this->id = id;
        this->fullName = fullName;
        this->icNumber = icNumber;
        this->phone = "";
        this->email = "";
        this->position = "";
        this->department = "";
        this->serviceLevel = "";
        this->status = "AKTIF";
    }

    string getId() const {
        return id;
    }

    string getFullName() const {
        return fullName;
    }

    string getIcNumber() const {
        return icNumber;
    }
};

// =====================================================
// Domain Entity: Transaction
// =====================================================

class Transaction {
private:
    string id;
    string transactionNo;
    string residentId;
    string paymentId;
    Date transactionDate;
    TransactionCategory category;
    TransactionStatus status;
    string relatedTransactionId;
    Decimal debitAmount;
    Decimal creditAmount;

public:
    Transaction() {
        id = "";
        transactionNo = "";
        residentId = "";
        paymentId = "";
        transactionDate = "";
        category = TransactionCategory::LAIN_LAIN;
        status = TransactionStatus::NORMAL;
        relatedTransactionId = "";
        debitAmount = 0.0;
        creditAmount = 0.0;
    }

    Transaction(
        string id,
        string transactionNo,
        string residentId,
        string paymentId,
        Date transactionDate,
        TransactionCategory category,
        TransactionStatus status,
        string relatedTransactionId,
        Decimal debitAmount,
        Decimal creditAmount
    ) {
        this->id = id;
        this->transactionNo = transactionNo;
        this->residentId = residentId;
        this->paymentId = paymentId;
        this->transactionDate = transactionDate;
        this->category = category;
        this->status = status;
        this->relatedTransactionId = relatedTransactionId;
        this->debitAmount = debitAmount;
        this->creditAmount = creditAmount;
    }

    string getId() const {
        return id;
    }

    string getTransactionNo() const {
        return transactionNo;
    }

    string getResidentId() const {
        return residentId;
    }

    string getPaymentId() const {
        return paymentId;
    }

    Date getTransactionDate() const {
        return transactionDate;
    }

    TransactionCategory getCategory() const {
        return category;
    }

    TransactionStatus getStatus() const {
        return status;
    }

    string getRelatedTransactionId() const {
        return relatedTransactionId;
    }

    Decimal getDebitAmount() const {
        return debitAmount;
    }

    Decimal getCreditAmount() const {
        return creditAmount;
    }

    Decimal getNetAmount() const {
        return debitAmount - creditAmount;
    }

    bool isReversible() const {
        return status == TransactionStatus::NORMAL ||
               status == TransactionStatus::DILARASKAN;
    }

    bool isAdjustable() const {
        return status == TransactionStatus::NORMAL;
    }

    Transaction markReversed() {
        status = TransactionStatus::DIBATALKAN;
        return *this;
    }

    Transaction markAdjusted() {
        status = TransactionStatus::DILARASKAN;
        return *this;
    }

    Transaction createReversal(string newTransactionNo, string remarks) const {
        return Transaction(
            "TXN_REV_" + id,
            newTransactionNo,
            residentId,
            paymentId,
            transactionDate,
            category,
            TransactionStatus::PEMBALIKAN,
            id,
            creditAmount,
            debitAmount
        );
    }

    Transaction createAdjustment(
        string newTransactionNo,
        Decimal newAmount,
        string remarks
    ) const {
        Decimal currentNetAmount = getNetAmount();
        Decimal difference = newAmount - currentNetAmount;

        Decimal adjustmentDebit = 0.0;
        Decimal adjustmentCredit = 0.0;

        if (difference > 0) {
            adjustmentDebit = difference;
        } else {
            adjustmentCredit = -difference;
        }

        return Transaction(
            "TXN_ADJ_" + id,
            newTransactionNo,
            residentId,
            paymentId,
            transactionDate,
            TransactionCategory::LAIN_LAIN,
            TransactionStatus::PELARASAN,
            id,
            adjustmentDebit,
            adjustmentCredit
        );
    }
};

// =====================================================
// Domain Entity: Payment
// =====================================================

class Payment {
private:
    string id;
    string residentId;
    Date paymentDate;
    string receiptNo;
    Decimal amount;
    string description;

public:
    Payment() {
        id = "";
        residentId = "";
        paymentDate = "";
        receiptNo = "";
        amount = 0.0;
        description = "";
    }

    Payment(
        string id,
        string residentId,
        Date paymentDate,
        string receiptNo,
        Decimal amount,
        string description
    ) {
        this->id = id;
        this->residentId = residentId;
        this->paymentDate = paymentDate;
        this->receiptNo = receiptNo;
        this->amount = amount;
        this->description = description;
    }

    Payment assignResident(string residentId) {
        this->residentId = residentId;
        return *this;
    }

    Payment updateDetails(Date paymentDate, string receiptNo, Decimal amount, string description) {
        this->paymentDate = paymentDate;
        this->receiptNo = receiptNo;
        this->amount = amount;
        this->description = description;
        return *this;
    }

    Transaction toTransaction(string transactionNo) {
        return Transaction(
            "TXN_" + id,
            transactionNo,
            residentId,
            id,
            paymentDate,
            TransactionCategory::BAYARAN,
            TransactionStatus::NORMAL,
            "",
            0.0,
            amount
        );
    }

    bool matchesReceipt(string receiptNo) const {
        return this->receiptNo == receiptNo;
    }

    bool isInMonth(Date monthStartDate, Date nextMonthStartDate) const {
        return paymentDate >= monthStartDate && paymentDate < nextMonthStartDate;
    }

    Decimal getAmount() const {
        return amount;
    }

    bool belongsToResident(string residentId) const {
        return this->residentId == residentId;
    }
};

// =====================================================
// Creational Pattern: Builder
// Class: TransactionBuilder
// =====================================================

class TransactionBuilder {
private:
    string transactionNo;
    string residentId;
    string paymentId;
    Date transactionDate;
    TransactionCategory category;
    TransactionStatus status;
    Decimal debitAmount;
    Decimal creditAmount;
    string relatedTransactionId;

public:
    TransactionBuilder() {
        transactionNo = "";
        residentId = "";
        paymentId = "";
        transactionDate = "";
        category = TransactionCategory::LAIN_LAIN;
        status = TransactionStatus::NORMAL;
        debitAmount = 0.0;
        creditAmount = 0.0;
        relatedTransactionId = "";
    }

    TransactionBuilder& withNumber(string transactionNo) {
        this->transactionNo = transactionNo;
        return *this;
    }

    TransactionBuilder& forResident(string residentId) {
        this->residentId = residentId;
        return *this;
    }

    TransactionBuilder& forPayment(string paymentId) {
        this->paymentId = paymentId;
        return *this;
    }

    TransactionBuilder& onDate(Date transactionDate) {
        this->transactionDate = transactionDate;
        return *this;
    }

    TransactionBuilder& withCategory(TransactionCategory category) {
        this->category = category;
        return *this;
    }

    TransactionBuilder& withStatus(TransactionStatus status) {
        this->status = status;
        return *this;
    }

    TransactionBuilder& withDebit(Decimal amount) {
        this->debitAmount = amount;
        return *this;
    }

    TransactionBuilder& withCredit(Decimal amount) {
        this->creditAmount = amount;
        return *this;
    }

    TransactionBuilder& relatedTo(string transactionId) {
        this->relatedTransactionId = transactionId;
        return *this;
    }

    Transaction build() {
        return Transaction(
            "TXN_" + transactionNo,
            transactionNo,
            residentId,
            paymentId,
            transactionDate,
            category,
            status,
            relatedTransactionId,
            debitAmount,
            creditAmount
        );
    }
};

// =====================================================
// Repository Interface
// Interface: IRepository
// =====================================================

template <typename T>
class IRepository {
public:
    virtual T findById(string id) = 0;
    virtual vector<T> findMany(QueryFilter filter) = 0;
    virtual T create(T input) = 0;
    virtual T update(string id, T input) = 0;
    virtual bool remove(string id) = 0;
    virtual ~IRepository() {}
};

// =====================================================
// Repository: TransactionRepository
// =====================================================

class TransactionRepository : public IRepository<Transaction> {
private:
    vector<Transaction> transactions;

public:
    TransactionRepository() {
        Transaction rentalCharge = TransactionBuilder()
            .withNumber("TRX-202606-0001")
            .forResident("RES_001")
            .onDate("2026-06-01")
            .withCategory(TransactionCategory::CAJ_SEWA)
            .withStatus(TransactionStatus::NORMAL)
            .withDebit(300.00)
            .build();

        Transaction payment = TransactionBuilder()
            .withNumber("TRX-202606-0002")
            .forResident("RES_001")
            .forPayment("PAY_001")
            .onDate("2026-06-05")
            .withCategory(TransactionCategory::BAYARAN)
            .withStatus(TransactionStatus::NORMAL)
            .withCredit(150.00)
            .build();

        transactions.push_back(rentalCharge);
        transactions.push_back(payment);
    }

    Transaction findById(string id) override {
        for (Transaction transaction : transactions) {
            if (transaction.getId() == id) {
                return transaction;
            }
        }

        return Transaction();
    }

    vector<Transaction> findMany(QueryFilter filter) override {
        vector<Transaction> result;

        for (Transaction transaction : transactions) {
            bool matchesSearch =
                filter.search.empty() ||
                transaction.getTransactionNo().find(filter.search) != string::npos ||
                transaction.getResidentId().find(filter.search) != string::npos;

            bool matchesStartDate =
                filter.startDate.empty() ||
                transaction.getTransactionDate() >= filter.startDate;

            bool matchesEndDate =
                filter.endDate.empty() ||
                transaction.getTransactionDate() <= filter.endDate;

            if (matchesSearch && matchesStartDate && matchesEndDate) {
                result.push_back(transaction);
            }
        }

        return result;
    }

    Transaction create(Transaction input) override {
        transactions.push_back(input);
        return input;
    }

    Transaction update(string id, Transaction input) override {
        for (int i = 0; i < transactions.size(); i++) {
            if (transactions[i].getId() == id) {
                transactions[i] = input;
                return transactions[i];
            }
        }

        return Transaction();
    }

    bool remove(string id) override {
        for (int i = 0; i < transactions.size(); i++) {
            if (transactions[i].getId() == id) {
                transactions.erase(transactions.begin() + i);
                return true;
            }
        }

        return false;
    }

    Transaction findByTransactionNo(string transactionNo) {
        for (Transaction transaction : transactions) {
            if (transaction.getTransactionNo() == transactionNo) {
                return transaction;
            }
        }

        return Transaction();
    }

    Transaction findByReceiptNo(string receiptNo) {
        for (Transaction transaction : transactions) {
            if (transaction.getPaymentId() == receiptNo) {
                return transaction;
            }
        }

        return Transaction();
    }

    PaginatedResult<Transaction> findList(TransactionFilterParams params) {
        vector<Transaction> filteredTransactions = findMany(params.toQuery());

        return {
            filteredTransactions,
            params.page,
            params.limit,
            static_cast<int>(filteredTransactions.size())
        };
    }

    Transaction save(Transaction transaction) {
        Transaction existing = findById(transaction.getId());

        if (existing.getId().empty()) {
            return create(transaction);
        }

        return update(transaction.getId(), transaction);
    }

    TransactionSummary getSummary() {
        Decimal totalDebit = 0.0;
        Decimal totalCredit = 0.0;

        for (Transaction transaction : transactions) {
            totalDebit += transaction.getDebitAmount();
            totalCredit += transaction.getCreditAmount();
        }

        return {
            static_cast<int>(transactions.size()),
            totalDebit,
            totalCredit
        };
    }
};

// =====================================================
// Mapper Interface
// Interface: EntityMapper
// =====================================================

template <typename T, typename D>
class EntityMapper {
public:
    virtual D toDto(T entity) = 0;
    virtual vector<D> toDtoList(vector<T> entities) = 0;
    virtual ~EntityMapper() {}
};

// =====================================================
// Mapper: TransactionMapper
// =====================================================

class TransactionMapper : public EntityMapper<Transaction, TransactionDto> {
public:
    TransactionDto toDto(Transaction entity) override {
        return {
            entity.getId(),
            entity.getTransactionNo(),
            entity.getResidentId(),
            entity.getPaymentId(),
            entity.getTransactionDate(),
            categoryToString(entity.getCategory()),
            statusToString(entity.getStatus()),
            entity.getDebitAmount(),
            entity.getCreditAmount(),
            entity.getNetAmount(),
            entity.getRelatedTransactionId()
        };
    }

    vector<TransactionDto> toDtoList(vector<Transaction> entities) override {
        vector<TransactionDto> result;

        for (Transaction transaction : entities) {
            result.push_back(toDto(transaction));
        }

        return result;
    }
};

// =====================================================
// Command Result
// =====================================================

struct CommandResult {
    bool success;
    string message;
    Transaction transaction;
};

// =====================================================
// Behavioral Pattern: Command
// Interface: ICommand
// =====================================================

class ICommand {
public:
    virtual CommandResult execute() = 0;
    virtual CommandResult undo() = 0;
    virtual string getDescription() = 0;
    virtual ~ICommand() {}
};

// =====================================================
// Concrete Command: ReverseTransactionCommand
// =====================================================

class ReverseTransactionCommand : public ICommand {
private:
    string transactionId;
    string remarks;
    AdminProfile actor;
    TransactionRepository& transactionRepository;
    Transaction reversalTransaction;

public:
    ReverseTransactionCommand(
        string transactionId,
        string remarks,
        AdminProfile actor,
        TransactionRepository& repository
    ) : transactionRepository(repository) {
        this->transactionId = transactionId;
        this->remarks = remarks;
        this->actor = actor;
    }

    CommandResult execute() override {
        Transaction original =
            transactionRepository.findById(transactionId);

        if (original.getId().empty()) {
            return {
                false,
                "Reverse failed. Transaction not found.",
                Transaction()
            };
        }

        if (!original.isReversible()) {
            return {
                false,
                "Reverse failed. Transaction is not reversible.",
                Transaction()
            };
        }

        string reversalNo = "REV-" + original.getTransactionNo();

        reversalTransaction =
            original.createReversal(reversalNo, remarks);

        Transaction reversedOriginal = original.markReversed();

        transactionRepository.update(original.getId(), reversedOriginal);
        transactionRepository.save(reversalTransaction);

        cout << "Transaction reversed by admin: "
             << actor.username << endl;

        return {
            true,
            "Transaction reversed successfully.",
            reversalTransaction
        };
    }

    CommandResult undo() override {
        if (reversalTransaction.getId().empty()) {
            return {
                false,
                "Undo failed. No reversal transaction was created.",
                Transaction()
            };
        }

        transactionRepository.remove(reversalTransaction.getId());

        return {
            true,
            "Reversal command undone.",
            reversalTransaction
        };
    }

    string getDescription() override {
        return "Reverse transaction " + transactionId + ". Remarks: " + remarks;
    }
};

// =====================================================
// Concrete Command: AdjustTransactionCommand
// =====================================================

class AdjustTransactionCommand : public ICommand {
private:
    string transactionId;
    Decimal newAmount;
    string remarks;
    AdminProfile actor;
    TransactionRepository& transactionRepository;
    Transaction adjustmentTransaction;

public:
    AdjustTransactionCommand(
        string transactionId,
        Decimal newAmount,
        string remarks,
        AdminProfile actor,
        TransactionRepository& repository
    ) : transactionRepository(repository) {
        this->transactionId = transactionId;
        this->newAmount = newAmount;
        this->remarks = remarks;
        this->actor = actor;
    }

    CommandResult execute() override {
        Transaction original =
            transactionRepository.findById(transactionId);

        if (original.getId().empty()) {
            return {
                false,
                "Adjustment failed. Transaction not found.",
                Transaction()
            };
        }

        if (!original.isAdjustable()) {
            return {
                false,
                "Adjustment failed. Transaction is not adjustable.",
                Transaction()
            };
        }

        string adjustmentNo = "ADJ-" + original.getTransactionNo();

        adjustmentTransaction =
            original.createAdjustment(adjustmentNo, newAmount, remarks);

        Transaction adjustedOriginal = original.markAdjusted();

        transactionRepository.update(original.getId(), adjustedOriginal);
        transactionRepository.save(adjustmentTransaction);

        cout << "Transaction adjusted by admin: "
             << actor.username << endl;

        return {
            true,
            "Transaction adjusted successfully.",
            adjustmentTransaction
        };
    }

    CommandResult undo() override {
        if (adjustmentTransaction.getId().empty()) {
            return {
                false,
                "Undo failed. No adjustment transaction was created.",
                Transaction()
            };
        }

        transactionRepository.remove(adjustmentTransaction.getId());

        return {
            true,
            "Adjustment command undone.",
            adjustmentTransaction
        };
    }

    string getDescription() override {
        return "Adjust transaction " + transactionId +
               " to new amount RM " + to_string(newAmount) +
               ". Remarks: " + remarks;
    }
};

// =====================================================
// Service Layer
// Class: TransactionService
// =====================================================

class TransactionService {
private:
    TransactionRepository& transactionRepository;

public:
    TransactionService(TransactionRepository& repository)
        : transactionRepository(repository) {
    }

    TransactionSummary getTransactionsSummary() {
        return transactionRepository.getSummary();
    }

    PaginatedResult<Transaction> getTransactionList(
        TransactionFilterParams params
    ) {
        return transactionRepository.findList(params);
    }

    string generateTransactionNo(Date date) {
        TransactionSummary summary = transactionRepository.getSummary();
        int nextNumber = summary.totalCount + 1;

        return "TRX-" + date + "-000" + to_string(nextNumber);
    }

    vector<string> generateTransactionNos(Date date, int count) {
        vector<string> transactionNos;

        for (int i = 0; i < count; i++) {
            transactionNos.push_back(
                "TRX-" + date + "-000" + to_string(i + 1)
            );
        }

        return transactionNos;
    }

    Transaction reverseTransaction(
        string transactionId,
        AdminProfile actor,
        string remarks
    ) {
        ReverseTransactionCommand command(
            transactionId,
            remarks,
            actor,
            transactionRepository
        );

        CommandResult result = executeCommand(command);
        return result.transaction;
    }

    Transaction adjustTransaction(
        string transactionId,
        AdminProfile actor,
        Decimal newAmount,
        string remarks
    ) {
        AdjustTransactionCommand command(
            transactionId,
            newAmount,
            remarks,
            actor,
            transactionRepository
        );

        CommandResult result = executeCommand(command);
        return result.transaction;
    }

    CommandResult executeCommand(ICommand& command) {
        cout << "Executing command: "
             << command.getDescription() << endl;

        return command.execute();
    }
};

// =====================================================
// Supporting DTOs for FinanceFacade
// =====================================================

struct TunggakanFilter {
    string keyword;
};

struct TunggakanListItem {
    string residentId;
    string residentName;
    Decimal outstandingAmount;
};

struct BulkUpdateTunggakanInput {
    vector<string> residentIds;
    Decimal amount;
    string description;
};

struct ArrearsSummary {
    int affectedResidentCount;
    Decimal totalAmount;
};

struct BillingCycle {
    Date billingMonth;
    bool success;
    int recordsBilled;
};

// =====================================================
// Structural Pattern: Facade
// Class: FinanceFacade
// =====================================================

class FinanceFacade {
private:
    TransactionService& transactionService;

public:
    FinanceFacade(TransactionService& transactionService)
        : transactionService(transactionService) {
    }

    vector<TunggakanListItem> getArrearsList(TunggakanFilter filter) {
        return {
            { "RES_001", "Ali Bin Abu", 300.00 },
            { "RES_002", "Siti Binti Ahmad", 120.00 }
        };
    }

    ArrearsSummary bulkUpdateArrears(
        BulkUpdateTunggakanInput input,
        AdminProfile actor
    ) {
        cout << "Bulk arrears updated by admin: "
             << actor.username << endl;

        return {
            static_cast<int>(input.residentIds.size()),
            input.amount * input.residentIds.size()
        };
    }

    Transaction reverseTransaction(
        string transactionId,
        string remarks,
        AdminProfile actor
    ) {
        return transactionService.reverseTransaction(
            transactionId,
            actor,
            remarks
        );
    }

    Transaction adjustTransaction(
        string transactionId,
        Decimal newAmount,
        string remarks,
        AdminProfile actor
    ) {
        return transactionService.adjustTransaction(
            transactionId,
            actor,
            newAmount,
            remarks
        );
    }

    BillingCycle runMonthlyBilling(
        Date month,
        AdminProfile actor
    ) {
        cout << "Monthly billing triggered by admin: "
             << actor.username << endl;

        return {
            month,
            true,
            10
        };
    }

    PaginatedResult<Transaction> getTransactionList(
        TransactionFilterParams params
    ) {
        return transactionService.getTransactionList(params);
    }

    TransactionSummary getTransactionsSummary() {
        return transactionService.getTransactionsSummary();
    }
};

// =====================================================
// Controller Layer
// Class: TransactionController
// =====================================================

class TransactionController {
private:
    FinanceFacade& financeFacade;
    TransactionMapper transactionMapper;

public:
    TransactionController(FinanceFacade& financeFacade)
        : financeFacade(financeFacade) {
    }

    ApiResponse<PaginatedResult<TransactionDto>> getTransactions(
        TransactionListRequest request
    ) {
        PaginatedResult<Transaction> transactionResult =
            financeFacade.getTransactionList(request.params);

        PaginatedResult<TransactionDto> dtoResult = {
            transactionMapper.toDtoList(transactionResult.items),
            transactionResult.page,
            transactionResult.limit,
            transactionResult.total
        };

        return {
            true,
            "Transaction list retrieved successfully.",
            dtoResult
        };
    }

    ApiResponse<TransactionDto> postAdjustTransaction(
        TransactionAdjustRequest request
    ) {
        Transaction transaction =
            financeFacade.adjustTransaction(
                request.transactionId,
                request.newAmount,
                request.remarks,
                request.actor
            );

        if (transaction.getId().empty()) {
            return {
                false,
                "Transaction adjustment failed.",
                TransactionDto()
            };
        }

        return {
            true,
            "Transaction adjusted successfully.",
            transactionMapper.toDto(transaction)
        };
    }

    ApiResponse<TransactionDto> postReverseTransaction(
        TransactionReverseRequest request
    ) {
        Transaction transaction =
            financeFacade.reverseTransaction(
                request.transactionId,
                request.remarks,
                request.actor
            );

        if (transaction.getId().empty()) {
            return {
                false,
                "Transaction reversal failed.",
                TransactionDto()
            };
        }

        return {
            true,
            "Transaction reversed successfully.",
            transactionMapper.toDto(transaction)
        };
    }
};

// =====================================================
// Main Function
// =====================================================

int main() {
    cout << fixed << setprecision(2);

    TransactionRepository transactionRepository;
    TransactionService transactionService(transactionRepository);
    FinanceFacade financeFacade(transactionService);
    TransactionController transactionController(financeFacade);

    AdminProfile admin = {
        "ADMIN_001",
        "financeAdmin"
    };

    TransactionListRequest listRequest = {
        {
            "",
            "2026-06-01",
            "2026-06-30",
            1,
            10
        }
    };

    ApiResponse<PaginatedResult<TransactionDto>> listResponse =
        transactionController.getTransactions(listRequest);

    cout << "===== Transaction List =====" << endl;
    cout << listResponse.message << endl;

    for (TransactionDto item : listResponse.data.items) {
        cout << item.transactionNo
             << " | Resident: " << item.residentId
             << " | Category: " << item.category
             << " | Status: " << item.status
             << " | Debit: RM " << item.debitAmount
             << " | Credit: RM " << item.creditAmount
             << " | Net: RM " << item.netAmount
             << endl;
    }

    TransactionAdjustRequest adjustRequest = {
        "TXN_TRX-202606-0001",
        350.00,
        "Rental amount adjusted after verification.",
        admin
    };

    ApiResponse<TransactionDto> adjustResponse =
        transactionController.postAdjustTransaction(adjustRequest);

    cout << "\n===== Adjustment Result =====" << endl;
    cout << adjustResponse.message << endl;

    if (adjustResponse.success) {
        cout << "Adjustment Transaction No: "
             << adjustResponse.data.transactionNo << endl;
        cout << "Debit: RM "
             << adjustResponse.data.debitAmount << endl;
        cout << "Credit: RM "
             << adjustResponse.data.creditAmount << endl;
        cout << "Related Transaction ID: "
             << adjustResponse.data.relatedTransactionId << endl;
    }

    TransactionReverseRequest reverseRequest = {
        "TXN_TRX-202606-0002",
        "Payment entered under wrong receipt reference.",
        admin
    };

    ApiResponse<TransactionDto> reverseResponse =
        transactionController.postReverseTransaction(reverseRequest);

    cout << "\n===== Reversal Result =====" << endl;
    cout << reverseResponse.message << endl;

    if (reverseResponse.success) {
        cout << "Reversal Transaction No: "
             << reverseResponse.data.transactionNo << endl;
        cout << "Debit: RM "
             << reverseResponse.data.debitAmount << endl;
        cout << "Credit: RM "
             << reverseResponse.data.creditAmount << endl;
        cout << "Related Transaction ID: "
             << reverseResponse.data.relatedTransactionId << endl;
    }

    TransactionSummary summary =
        financeFacade.getTransactionsSummary();

    cout << "\n===== Transaction Summary =====" << endl;
    cout << "Total Transactions: "
         << summary.totalCount << endl;
    cout << "Total Debit: RM "
         << summary.totalDebit << endl;
    cout << "Total Credit: RM "
         << summary.totalCredit << endl;
    cout << "Net Total: RM "
         << summary.getNetTotal() << endl;

    return 0;
}