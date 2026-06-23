#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

// =====================================================
// Basic Type Aliases
// =====================================================

using Date = string;
using Decimal = double;

// =====================================================
// Enumerations
// =====================================================

enum class UnitStatus {
    OCCUPIED,
    VACANT
};

enum class TransactionStatus {
    DRAFT,
    POSTED
};

enum class TransactionCategory {
    MONTHLY_BILLING,
    PAYMENT,
    ADJUSTMENT,
    REVERSAL
};

string unitStatusToString(UnitStatus status) {
    return status == UnitStatus::OCCUPIED ? "OCCUPIED" : "VACANT";
}

string transactionStatusToString(TransactionStatus status) {
    return status == TransactionStatus::POSTED ? "POSTED" : "DRAFT";
}

// =====================================================
// DTOs and Request Models
// =====================================================

struct AdminProfile {
    string id;
    string username;
};

struct ApiResponseBase {
    bool success;
    string message;
};

template <typename T>
struct ApiResponse {
    bool success;
    string message;
    T data;
};

struct BillingStatusDto {
    string billingMonth;
    bool success;
    int recordsBilled;
};

struct BillingStatusRequest {
    string billingMonth;
};

struct BillingCronRequest {
    string billingMonth;
    AdminProfile actor;
};

struct ArrearsListRequest {
    string keyword;
};

struct ArrearsDetailRequest {
    string residentId;
};

struct BulkUpdateTunggakanInput {
    vector<string> residentIds;
    Decimal amount;
    string description;
};

struct BulkTunggakanInput {
    vector<string> residentIds;
    Decimal amount;
    string description;
};

struct AdditionalChargeInput {
    Decimal amount;
    string description;
};

struct RebateInput {
    Decimal amount;
    string description;
};

struct TunggakanFilter {
    string keyword;
};

struct TunggakanListItem {
    string residentId;
    string residentName;
    Decimal outstandingAmount;
};

struct ResidentTunggakanDetails {
    string residentId;
    string residentName;
    Decimal outstandingAmount;
    vector<string> remarks;
};

struct ArrearsSummaryDto {
    int affectedResidentCount;
    Decimal totalOutstandingAmount;
};

struct CommandResult {
    bool success;
    string message;
};

// =====================================================
// Transaction Entity
// =====================================================

class Transaction {
private:
    string transactionNo;
    string residentId;
    string paymentId;
    TransactionCategory category;
    TransactionStatus status;
    Decimal debitAmount;
    Decimal creditAmount;
    string relatedTransactionId;

public:
    Transaction() {
        transactionNo = "";
        residentId = "";
        paymentId = "";
        category = TransactionCategory::ADJUSTMENT;
        status = TransactionStatus::DRAFT;
        debitAmount = 0.0;
        creditAmount = 0.0;
        relatedTransactionId = "";
    }

    Transaction(
        string transactionNo,
        string residentId,
        string paymentId,
        TransactionCategory category,
        TransactionStatus status,
        Decimal debitAmount,
        Decimal creditAmount,
        string relatedTransactionId
    ) {
        this->transactionNo = transactionNo;
        this->residentId = residentId;
        this->paymentId = paymentId;
        this->category = category;
        this->status = status;
        this->debitAmount = debitAmount;
        this->creditAmount = creditAmount;
        this->relatedTransactionId = relatedTransactionId;
    }

    string getTransactionNo() const {
        return transactionNo;
    }

    string getResidentId() const {
        return residentId;
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

    void display() const {
        cout << "Transaction No: " << transactionNo << endl;
        cout << "Resident ID: " << residentId << endl;
        cout << "Debit: RM " << debitAmount << endl;
        cout << "Credit: RM " << creditAmount << endl;
        cout << "Status: " << transactionStatusToString(status) << endl;
    }
};

// =====================================================
// Domain Entity: BillingCycle
// =====================================================

class BillingCycle {
private:
    string id;
    Date billingMonth;
    Date runDate;
    bool success;
    int recordsBilled;

public:
    BillingCycle() {
        id = "";
        billingMonth = "";
        runDate = "";
        success = false;
        recordsBilled = 0;
    }

    BillingCycle(string id, Date billingMonth, Date runDate) {
        this->id = id;
        this->billingMonth = billingMonth;
        this->runDate = runDate;
        this->success = false;
        this->recordsBilled = 0;
    }

    void markSuccess(int recordsBilled) {
        this->success = true;
        this->recordsBilled = recordsBilled;
    }

    void markFailed() {
        this->success = false;
        this->recordsBilled = 0;
    }

    bool isForMonth(Date month) const {
        return billingMonth == month;
    }

    string getBillingMonth() const {
        return billingMonth;
    }

    bool getSuccess() const {
        return success;
    }

    int getRecordsBilled() const {
        return recordsBilled;
    }
};

// =====================================================
// Domain Entity: Unit
// =====================================================

class Unit {
private:
    string id;
    string unitCode;
    UnitStatus status;
    string categoryId;

public:
    Unit() {
        id = "";
        unitCode = "";
        status = UnitStatus::VACANT;
        categoryId = "";
    }

    Unit(string id, string unitCode, UnitStatus status, string categoryId) {
        this->id = id;
        this->unitCode = unitCode;
        this->status = status;
        this->categoryId = categoryId;
    }

    Unit rename(string newUnitCode) {
        unitCode = newUnitCode;
        return *this;
    }

    void assignResident(string residentId, Date moveInDate, Date moveOutDate) {
        status = UnitStatus::OCCUPIED;
        cout << "Resident " << residentId << " assigned to unit " << unitCode << endl;
    }

    void vacate(Date moveOutDate, string description) {
        status = UnitStatus::VACANT;
        cout << "Unit " << unitCode << " vacated. Reason: " << description << endl;
    }

    string getCurrentOccupancy() const {
        return status == UnitStatus::OCCUPIED ? "Currently occupied" : "Currently vacant";
    }

    vector<string> getOccupancyHistory() const {
        return { "Sample occupancy history record" };
    }

    bool isOccupied() const {
        return status == UnitStatus::OCCUPIED;
    }

    bool isVacant() const {
        return status == UnitStatus::VACANT;
    }
};

// =====================================================
// Domain Entity: Resident
// =====================================================

class Resident {
private:
    string id;
    string fullName;
    string icNumber;
    Decimal outstandingBalance;

public:
    Resident() {
        id = "";
        fullName = "";
        icNumber = "";
        outstandingBalance = 0.0;
    }

    Resident(string id, string fullName, string icNumber) {
        this->id = id;
        this->fullName = fullName;
        this->icNumber = icNumber;
        this->outstandingBalance = 0.0;
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

    void addOutstandingAmount(Decimal amount) {
        outstandingBalance += amount;
    }

    void deductOutstandingAmount(Decimal amount) {
        outstandingBalance -= amount;

        if (outstandingBalance < 0) {
            outstandingBalance = 0;
        }
    }

    Decimal getOutstandingBalance() const {
        return outstandingBalance;
    }
};

// =====================================================
// Domain Entity: AdditionalCharge
// =====================================================

class AdditionalCharge {
private:
    string id;
    string monthlyChargeId;
    Date chargeDate;
    string description;
    Decimal amount;

public:
    AdditionalCharge() {
        id = "";
        monthlyChargeId = "";
        chargeDate = "";
        description = "";
        amount = 0.0;
    }

    AdditionalCharge(
        string id,
        string monthlyChargeId,
        Date chargeDate,
        string description,
        Decimal amount
    ) {
        this->id = id;
        this->monthlyChargeId = monthlyChargeId;
        this->chargeDate = chargeDate;
        this->description = description;
        this->amount = amount;
    }

    AdditionalCharge update(string description, Decimal amount, Date chargeDate) {
        this->description = description;
        this->amount = amount;
        this->chargeDate = chargeDate;
        return *this;
    }

    Transaction toTransaction(string residentId) {
        return Transaction(
            "TRX-ADD-" + id,
            residentId,
            "",
            TransactionCategory::ADJUSTMENT,
            TransactionStatus::POSTED,
            amount,
            0.0,
            ""
        );
    }

    Decimal getAmount() const {
        return amount;
    }

    string getDescription() const {
        return description;
    }
};

// =====================================================
// Domain Entity: Rebate
// =====================================================

class Rebate {
private:
    string id;
    string monthlyChargeId;
    Date rebateDate;
    string description;
    Decimal amount;

public:
    Rebate() {
        id = "";
        monthlyChargeId = "";
        rebateDate = "";
        description = "";
        amount = 0.0;
    }

    Rebate(
        string id,
        string monthlyChargeId,
        Date rebateDate,
        string description,
        Decimal amount
    ) {
        this->id = id;
        this->monthlyChargeId = monthlyChargeId;
        this->rebateDate = rebateDate;
        this->description = description;
        this->amount = amount;
    }

    Rebate update(string description, Decimal amount, Date rebateDate) {
        this->description = description;
        this->amount = amount;
        this->rebateDate = rebateDate;
        return *this;
    }

    Transaction toTransaction(string residentId) {
        return Transaction(
            "TRX-REB-" + id,
            residentId,
            "",
            TransactionCategory::ADJUSTMENT,
            TransactionStatus::POSTED,
            0.0,
            amount,
            ""
        );
    }

    Decimal getAmount() const {
        return amount;
    }

    string getDescription() const {
        return description;
    }
};

// =====================================================
// Domain Entity: MonthlyCharge
// =====================================================

class MonthlyCharge {
private:
    string id;
    string residentId;
    string unitId;
    Date chargeMonth;
    Decimal rentalAmount;
    Decimal maintenanceAmount;
    Decimal penaltyAmount;
    Decimal additionalChargesTotal;
    Decimal rebateTotal;
    Decimal totalMonthlyCharge;
    Decimal paymentReceived;
    Decimal balanceForMonth;
    vector<AdditionalCharge> additionalCharges;
    vector<Rebate> rebates;

public:
    MonthlyCharge() {
        id = "";
        residentId = "";
        unitId = "";
        chargeMonth = "";
        rentalAmount = 0.0;
        maintenanceAmount = 0.0;
        penaltyAmount = 0.0;
        additionalChargesTotal = 0.0;
        rebateTotal = 0.0;
        totalMonthlyCharge = 0.0;
        paymentReceived = 0.0;
        balanceForMonth = 0.0;
    }

    MonthlyCharge(
        string id,
        string residentId,
        string unitId,
        Date chargeMonth,
        Decimal rentalAmount,
        Decimal maintenanceAmount,
        Decimal penaltyAmount
    ) {
        this->id = id;
        this->residentId = residentId;
        this->unitId = unitId;
        this->chargeMonth = chargeMonth;
        this->rentalAmount = rentalAmount;
        this->maintenanceAmount = maintenanceAmount;
        this->penaltyAmount = penaltyAmount;
        this->additionalChargesTotal = 0.0;
        this->rebateTotal = 0.0;
        this->totalMonthlyCharge = 0.0;
        this->paymentReceived = 0.0;
        this->balanceForMonth = 0.0;
    }

    Decimal calculateTotalCharge() {
        totalMonthlyCharge =
            rentalAmount + maintenanceAmount + penaltyAmount
            + additionalChargesTotal - rebateTotal;

        balanceForMonth = totalMonthlyCharge - paymentReceived;

        return totalMonthlyCharge;
    }

    void applyPayment(Decimal paymentAmount) {
        paymentReceived += paymentAmount;
        balanceForMonth = totalMonthlyCharge - paymentReceived;

        if (balanceForMonth < 0) {
            balanceForMonth = 0;
        }
    }

    AdditionalCharge addAdditionalCharge(
        string description,
        Decimal amount,
        Date chargeDate
    ) {
        AdditionalCharge charge(
            "ADD_" + to_string(additionalCharges.size() + 1),
            id,
            chargeDate,
            description,
            amount
        );

        additionalCharges.push_back(charge);
        additionalChargesTotal += amount;
        calculateTotalCharge();

        return charge;
    }

    Rebate addRebate(
        string description,
        Decimal amount,
        Date rebateDate
    ) {
        Rebate rebate(
            "REB_" + to_string(rebates.size() + 1),
            id,
            rebateDate,
            description,
            amount
        );

        rebates.push_back(rebate);
        rebateTotal += amount;
        calculateTotalCharge();

        return rebate;
    }

    Decimal calculateBalance() {
        calculateTotalCharge();
        return balanceForMonth;
    }

    bool isFullyPaid() {
        return balanceForMonth <= 0;
    }

    string getId() const {
        return id;
    }

    string getResidentId() const {
        return residentId;
    }

    Decimal getTotalMonthlyCharge() const {
        return totalMonthlyCharge;
    }

    Decimal getBalanceForMonth() const {
        return balanceForMonth;
    }
};

// =====================================================
// Domain Entity: ArrearsSummary
// =====================================================

class ArrearsSummary {
private:
    string id;
    string residentId;
    Decimal totalArrearsAmount;
    Date lastUpdatedMonth;

public:
    ArrearsSummary() {
        id = "";
        residentId = "";
        totalArrearsAmount = 0.0;
        lastUpdatedMonth = "";
    }

    ArrearsSummary(string id, string residentId) {
        this->id = id;
        this->residentId = residentId;
        this->totalArrearsAmount = 0.0;
        this->lastUpdatedMonth = "";
    }

    void recalculate(vector<MonthlyCharge> charges, vector<Transaction> payments) {
        totalArrearsAmount = 0.0;

        for (MonthlyCharge charge : charges) {
            totalArrearsAmount += charge.getBalanceForMonth();
        }

        for (Transaction payment : payments) {
            totalArrearsAmount -= payment.getCreditAmount();
        }

        if (totalArrearsAmount < 0) {
            totalArrearsAmount = 0;
        }
    }

    void updateAmount(Decimal numberAmount, Date lastUpdatedMonth, string description) {
        this->totalArrearsAmount = numberAmount;
        this->lastUpdatedMonth = lastUpdatedMonth;

        cout << "Arrears updated. Reason: " << description << endl;
    }

    bool isOverdue() const {
        return totalArrearsAmount > 0;
    }

    bool isOverpaid() const {
        return totalArrearsAmount < 0;
    }

    bool hasOutstandingAmount() const {
        return totalArrearsAmount > 0;
    }

    vector<TunggakanListItem> toListItem(Resident resident) {
        return {
            {
                resident.getId(),
                resident.getFullName(),
                totalArrearsAmount
            }
        };
    }

    Decimal getAmount() const {
        return totalArrearsAmount;
    }
};

// =====================================================
// Creational Pattern: Builder
// Class: MonthlyChargeBuilder
// =====================================================

class MonthlyChargeBuilder {
private:
    string residentId;
    string unitId;
    Date chargeMonth;
    Decimal rentalAmount;
    Decimal maintenanceAmount;
    Decimal penaltyAmount;
    vector<AdditionalCharge> additionalCharges;
    vector<Rebate> rebates;

public:
    MonthlyChargeBuilder() {
        residentId = "";
        unitId = "";
        chargeMonth = "";
        rentalAmount = 0.0;
        maintenanceAmount = 0.0;
        penaltyAmount = 0.0;
    }

    MonthlyChargeBuilder& forResident(string residentId) {
        this->residentId = residentId;
        return *this;
    }

    MonthlyChargeBuilder& forUnit(string unitId) {
        this->unitId = unitId;
        return *this;
    }

    MonthlyChargeBuilder& forMonth(Date chargeMonth) {
        this->chargeMonth = chargeMonth;
        return *this;
    }

    MonthlyChargeBuilder& withRates(
        Decimal rental,
        Decimal maintenance,
        Decimal penalty
    ) {
        this->rentalAmount = rental;
        this->maintenanceAmount = maintenance;
        this->penaltyAmount = penalty;
        return *this;
    }

    MonthlyChargeBuilder& withAdditionalCharges(vector<AdditionalCharge> charges) {
        this->additionalCharges = charges;
        return *this;
    }

    MonthlyChargeBuilder& withRebates(vector<Rebate> rebates) {
        this->rebates = rebates;
        return *this;
    }

    MonthlyCharge build() {
        MonthlyCharge charge(
            "MC_" + residentId + "_" + chargeMonth,
            residentId,
            unitId,
            chargeMonth,
            rentalAmount,
            maintenanceAmount,
            penaltyAmount
        );

        for (AdditionalCharge item : additionalCharges) {
            charge.addAdditionalCharge(
                item.getDescription(),
                item.getAmount(),
                chargeMonth
            );
        }

        for (Rebate item : rebates) {
            charge.addRebate(
                item.getDescription(),
                item.getAmount(),
                chargeMonth
            );
        }

        charge.calculateTotalCharge();
        return charge;
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
        category = TransactionCategory::ADJUSTMENT;
        status = TransactionStatus::DRAFT;
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

    TransactionBuilder& withCategory(TransactionCategory category) {
        this->category = category;
        return *this;
    }

    TransactionBuilder& withStatus(TransactionStatus status) {
        this->status = status;
        return *this;
    }

    TransactionBuilder& withDebit(Decimal debitAmount) {
        this->debitAmount = debitAmount;
        return *this;
    }

    TransactionBuilder& withCredit(Decimal creditAmount) {
        this->creditAmount = creditAmount;
        return *this;
    }

    TransactionBuilder& relatedTo(string transactionId) {
        this->relatedTransactionId = transactionId;
        return *this;
    }

    Transaction build() {
        return Transaction(
            transactionNo,
            residentId,
            paymentId,
            category,
            status,
            debitAmount,
            creditAmount,
            relatedTransactionId
        );
    }
};

// =====================================================
// Service: BillingService
// =====================================================

class BillingService {
private:
    vector<BillingCycle> billingCycles;
    vector<MonthlyCharge> monthlyCharges;

public:
    BillingCycle runMonthlyBilling(Date billingMonth, AdminProfile actor) {
        cout << "Monthly billing started by admin: " << actor.username << endl;

        BillingCycle cycle(
            "BILL_" + billingMonth,
            billingMonth,
            "2026-06-23"
        );

        MonthlyCharge charge = MonthlyChargeBuilder()
            .forResident("RES_001")
            .forUnit("UNIT_A_01")
            .forMonth(billingMonth)
            .withRates(300.00, 50.00, 0.00)
            .build();

        monthlyCharges.push_back(charge);

        cycle.markSuccess(1);
        billingCycles.push_back(cycle);

        return cycle;
    }

    BillingStatusDto getBillingStatus(Date billingMonth) {
        for (BillingCycle cycle : billingCycles) {
            if (cycle.isForMonth(billingMonth)) {
                return {
                    cycle.getBillingMonth(),
                    cycle.getSuccess(),
                    cycle.getRecordsBilled()
                };
            }
        }

        return {
            billingMonth,
            false,
            0
        };
    }

    vector<MonthlyCharge> createMonthlyCharges(Date billingMonth, vector<Resident> residents) {
        vector<MonthlyCharge> charges;

        for (Resident resident : residents) {
            MonthlyCharge charge = MonthlyChargeBuilder()
                .forResident(resident.getId())
                .forUnit("UNIT_A_01")
                .forMonth(billingMonth)
                .withRates(300.00, 50.00, 0.00)
                .build();

            charges.push_back(charge);
            monthlyCharges.push_back(charge);
        }

        return charges;
    }

    BillingCycle recordBillingCycle(
        Date billingMonth,
        bool success,
        int recordsBilled
    ) {
        BillingCycle cycle(
            "BILL_" + billingMonth,
            billingMonth,
            "2026-06-23"
        );

        if (success) {
            cycle.markSuccess(recordsBilled);
        } else {
            cycle.markFailed();
        }

        billingCycles.push_back(cycle);
        return cycle;
    }
};

// =====================================================
// Service: ArrearsService
// =====================================================

class ArrearsService {
private:
    vector<Resident> residents;
    vector<ArrearsSummary> arrearsSummaries;

public:
    ArrearsService() {
        Resident resident1("RES_001", "Ali Bin Abu", "660101011111");
        resident1.addOutstandingAmount(350.00);

        Resident resident2("RES_002", "Siti Binti Ahmad", "900505015555");
        resident2.addOutstandingAmount(120.00);

        residents.push_back(resident1);
        residents.push_back(resident2);

        ArrearsSummary summary1("ARR_001", "RES_001");
        summary1.updateAmount(350.00, "2026-06", "Initial arrears record");

        ArrearsSummary summary2("ARR_002", "RES_002");
        summary2.updateAmount(120.00, "2026-06", "Initial arrears record");

        arrearsSummaries.push_back(summary1);
        arrearsSummaries.push_back(summary2);
    }

    vector<TunggakanListItem> getArrearsList(TunggakanFilter filter) {
        vector<TunggakanListItem> result;

        for (Resident resident : residents) {
            if (filter.keyword.empty() ||
                resident.getFullName().find(filter.keyword) != string::npos) {
                result.push_back({
                    resident.getId(),
                    resident.getFullName(),
                    resident.getOutstandingBalance()
                });
            }
        }

        return result;
    }

    ResidentTunggakanDetails getArrearsDetails(string residentId) {
        for (Resident resident : residents) {
            if (resident.getId() == residentId) {
                return {
                    resident.getId(),
                    resident.getFullName(),
                    resident.getOutstandingBalance(),
                    { "Outstanding arrears retrieved from arrears summary." }
                };
            }
        }

        return {
            "",
            "",
            0.0,
            { "Resident not found." }
        };
    }

    BulkTunggakanInput parseBulkUpdateBody(BulkUpdateTunggakanInput body) {
        return {
            body.residentIds,
            body.amount,
            body.description
        };
    }

    Transaction applyAdditionalCharge(
        string residentId,
        AdditionalChargeInput input,
        AdminProfile actor
    ) {
        cout << "Additional charge applied by admin: " << actor.username << endl;

        return TransactionBuilder()
            .withNumber("TRX-ADD-" + residentId)
            .forResident(residentId)
            .withCategory(TransactionCategory::ADJUSTMENT)
            .withStatus(TransactionStatus::POSTED)
            .withDebit(input.amount)
            .build();
    }

    Transaction applyRebate(
        string residentId,
        RebateInput input,
        AdminProfile actor
    ) {
        cout << "Rebate applied by admin: " << actor.username << endl;

        return TransactionBuilder()
            .withNumber("TRX-REB-" + residentId)
            .forResident(residentId)
            .withCategory(TransactionCategory::ADJUSTMENT)
            .withStatus(TransactionStatus::POSTED)
            .withCredit(input.amount)
            .build();
    }

    ArrearsSummary recalculateResidentArrears(string residentId) {
        for (Resident resident : residents) {
            if (resident.getId() == residentId) {
                ArrearsSummary summary("ARR_" + residentId, residentId);
                summary.updateAmount(
                    resident.getOutstandingBalance(),
                    "2026-06",
                    "Recalculated resident arrears"
                );

                return summary;
            }
        }

        return ArrearsSummary();
    }
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
// Concrete Command: RunBillingCommand
// =====================================================

class RunBillingCommand : public ICommand {
private:
    Date billingMonth;
    AdminProfile actor;
    BillingService& billingService;
    BillingCycle executedCycle;

public:
    RunBillingCommand(
        Date billingMonth,
        AdminProfile actor,
        BillingService& billingService
    ) : billingService(billingService) {
        this->billingMonth = billingMonth;
        this->actor = actor;
    }

    CommandResult execute() override {
        executedCycle = billingService.runMonthlyBilling(billingMonth, actor);

        return {
            executedCycle.getSuccess(),
            "Monthly billing command executed for " + billingMonth
        };
    }

    CommandResult undo() override {
        executedCycle.markFailed();

        return {
            true,
            "Monthly billing command was undone for " + billingMonth
        };
    }

    string getDescription() override {
        return "Run monthly billing for " + billingMonth;
    }
};

// =====================================================
// Structural Pattern: Facade
// Class: FinanceFacade
// =====================================================

class FinanceFacade {
private:
    ArrearsService& arrearsService;
    BillingService& billingService;

public:
    FinanceFacade(
        ArrearsService& arrearsService,
        BillingService& billingService
    ) : arrearsService(arrearsService),
        billingService(billingService) {
    }

    vector<TunggakanListItem> getArrearsList(TunggakanFilter filter) {
        return arrearsService.getArrearsList(filter);
    }

    ArrearsSummaryDto bulkUpdateArrears(
        BulkTunggakanInput input,
        AdminProfile actor
    ) {
        cout << "Bulk arrears update performed by admin: "
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
        cout << "Transaction reversed by admin: "
             << actor.username << endl;

        return TransactionBuilder()
            .withNumber("TRX-REV-" + transactionId)
            .withCategory(TransactionCategory::REVERSAL)
            .withStatus(TransactionStatus::POSTED)
            .withCredit(100.00)
            .relatedTo(transactionId)
            .build();
    }

    Transaction adjustTransaction(
        string transactionId,
        Decimal newAmount,
        string remarks,
        AdminProfile actor
    ) {
        cout << "Transaction adjusted by admin: "
             << actor.username << endl;

        return TransactionBuilder()
            .withNumber("TRX-ADJ-" + transactionId)
            .withCategory(TransactionCategory::ADJUSTMENT)
            .withStatus(TransactionStatus::POSTED)
            .withDebit(newAmount)
            .relatedTo(transactionId)
            .build();
    }

    BillingCycle runMonthlyBilling(
        Date month,
        AdminProfile actor
    ) {
        RunBillingCommand command(month, actor, billingService);
        CommandResult result = command.execute();

        cout << result.message << endl;

        return billingService.recordBillingCycle(month, result.success, 1);
    }
};

// =====================================================
// Controller: BillingController
// =====================================================

class BillingController {
private:
    FinanceFacade& financeFacade;
    BillingService& billingService;

public:
    BillingController(
        FinanceFacade& financeFacade,
        BillingService& billingService
    ) : financeFacade(financeFacade),
        billingService(billingService) {
    }

    ApiResponse<BillingStatusDto> getBillingStatus(
        BillingStatusRequest request
    ) {
        BillingStatusDto status =
            billingService.getBillingStatus(request.billingMonth);

        return {
            true,
            "Billing status retrieved successfully.",
            status
        };
    }

    ApiResponse<BillingCycle> runBillingCron(
        BillingCronRequest request
    ) {
        BillingCycle cycle =
            financeFacade.runMonthlyBilling(
                request.billingMonth,
                request.actor
            );

        return {
            cycle.getSuccess(),
            "Billing cron executed.",
            cycle
        };
    }
};

// =====================================================
// Controller: ArrearsController
// =====================================================

class ArrearsController {
private:
    FinanceFacade& financeFacade;
    ArrearsService& arrearsService;

public:
    ArrearsController(
        FinanceFacade& financeFacade,
        ArrearsService& arrearsService
    ) : financeFacade(financeFacade),
        arrearsService(arrearsService) {
    }

    ApiResponse<vector<TunggakanListItem>> getArrears(
        ArrearsListRequest request
    ) {
        vector<TunggakanListItem> list =
            financeFacade.getArrearsList({ request.keyword });

        return {
            true,
            "Arrears list retrieved successfully.",
            list
        };
    }

    ApiResponse<ArrearsSummaryDto> postBulkUpdateArrears(
        BulkUpdateTunggakanInput request,
        AdminProfile actor
    ) {
        BulkTunggakanInput input =
            arrearsService.parseBulkUpdateBody(request);

        ArrearsSummaryDto result =
            financeFacade.bulkUpdateArrears(input, actor);

        return {
            true,
            "Bulk arrears update completed.",
            result
        };
    }

    ApiResponse<ResidentTunggakanDetails> getArrearsById(
        ArrearsDetailRequest request
    ) {
        ResidentTunggakanDetails details =
            arrearsService.getArrearsDetails(request.residentId);

        if (details.residentId.empty()) {
            return {
                false,
                "Resident arrears details not found.",
                details
            };
        }

        return {
            true,
            "Resident arrears details retrieved successfully.",
            details
        };
    }
};

// =====================================================
// Main Function
// =====================================================

int main() {
    cout << fixed << setprecision(2);

    AdminProfile admin = {
        "ADMIN_001",
        "financeAdmin"
    };

    ArrearsService arrearsService;
    BillingService billingService;
    FinanceFacade financeFacade(arrearsService, billingService);

    BillingController billingController(financeFacade, billingService);
    ArrearsController arrearsController(financeFacade, arrearsService);

    BillingCronRequest billingRequest = {
        "2026-06",
        admin
    };

    ApiResponse<BillingCycle> billingResponse =
        billingController.runBillingCron(billingRequest);

    cout << "\n===== Billing Cron Result =====" << endl;
    cout << billingResponse.message << endl;

    BillingStatusRequest statusRequest = {
        "2026-06"
    };

    ApiResponse<BillingStatusDto> statusResponse =
        billingController.getBillingStatus(statusRequest);

    cout << "\n===== Billing Status =====" << endl;
    cout << "Billing Month: "
         << statusResponse.data.billingMonth << endl;
    cout << "Success: "
         << (statusResponse.data.success ? "Yes" : "No") << endl;
    cout << "Records Billed: "
         << statusResponse.data.recordsBilled << endl;

    ArrearsListRequest arrearsListRequest = {
        ""
    };

    ApiResponse<vector<TunggakanListItem>> arrearsResponse =
        arrearsController.getArrears(arrearsListRequest);

    cout << "\n===== Arrears List =====" << endl;

    for (TunggakanListItem item : arrearsResponse.data) {
        cout << "Resident: " << item.residentName
             << " | Outstanding: RM "
             << item.outstandingAmount << endl;
    }

    ArrearsDetailRequest detailRequest = {
        "RES_001"
    };

    ApiResponse<ResidentTunggakanDetails> detailResponse =
        arrearsController.getArrearsById(detailRequest);

    cout << "\n===== Resident Arrears Details =====" << endl;
    cout << "Resident Name: "
         << detailResponse.data.residentName << endl;
    cout << "Outstanding Amount: RM "
         << detailResponse.data.outstandingAmount << endl;

    BulkUpdateTunggakanInput bulkInput = {
        { "RES_001", "RES_002" },
        50.00,
        "Manual arrears correction"
    };

    ApiResponse<ArrearsSummaryDto> bulkResponse =
        arrearsController.postBulkUpdateArrears(bulkInput, admin);

    cout << "\n===== Bulk Update Result =====" << endl;
    cout << "Affected Residents: "
         << bulkResponse.data.affectedResidentCount << endl;
    cout << "Total Adjustment Amount: RM "
         << bulkResponse.data.totalOutstandingAmount << endl;

    return 0;
}