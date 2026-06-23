#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <algorithm>

using namespace std;

using Date = string;
using Decimal = double;

// =====================================================
// Enumerations
// =====================================================

enum class ResidentStatus {
    AKTIF,
    TIDAK_LAYAK,
    PENCEN_MENDATANG,
    DATA_TIDAK_LENGKAP
};

enum class PaymentStatusFilter {
    KURANG,
    LEBIH,
    TIDAK_LENGKAP,
    CUKUP
};

enum class PaymentTone {
    GREEN,
    RED,
    BLUE,
    PURPLE
};

string residentStatusToString(ResidentStatus status) {
    switch (status) {
        case ResidentStatus::AKTIF:
            return "AKTIF";
        case ResidentStatus::TIDAK_LAYAK:
            return "TIDAK_LAYAK";
        case ResidentStatus::PENCEN_MENDATANG:
            return "PENCEN_MENDATANG";
        case ResidentStatus::DATA_TIDAK_LENGKAP:
            return "DATA_TIDAK_LENGKAP";
        default:
            return "UNKNOWN";
    }
}

string paymentStatusToString(PaymentStatusFilter status) {
    switch (status) {
        case PaymentStatusFilter::KURANG:
            return "KURANG";
        case PaymentStatusFilter::LEBIH:
            return "LEBIH";
        case PaymentStatusFilter::TIDAK_LENGKAP:
            return "TIDAK_LENGKAP";
        case PaymentStatusFilter::CUKUP:
            return "CUKUP";
        default:
            return "UNKNOWN";
    }
}

string paymentToneToString(PaymentTone tone) {
    switch (tone) {
        case PaymentTone::GREEN:
            return "GREEN";
        case PaymentTone::RED:
            return "RED";
        case PaymentTone::BLUE:
            return "BLUE";
        case PaymentTone::PURPLE:
            return "PURPLE";
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

template <typename T>
struct ApiResponse {
    bool success;
    string message;
    T data;
};

struct PaymentListRequest {
    Date paymentMonth;
    string search;
    vector<PaymentStatusFilter> statusFilters;
};

struct PaymentDetailRequest {
    string paymentId;
    bool includeHistory;
};

struct ManualPaymentRequest {
    string residentId;
    Date paymentDate;
    string receiptNo;
    Decimal amount;
    string description;
    AdminProfile actor;
};

struct PaymentCreateInput {
    string residentId;
    Date paymentDate;
    string receiptNo;
    Decimal amount;
    string description;
};

struct PaymentFilters {
    string search;
    vector<PaymentStatusFilter> statuses;
};

struct PaymentExportRow {
    string residentName;
    string icNumber;
    string unit;
    string status;
    string amount;
};

struct PaymentStats {
    int kurangCount;
    int lebihCount;
    int tidakLengkapCount;
    int cukupCount;
};

struct PaymentStatsQueryRow {
    int totalResidents;
    int totalPayments;
    Decimal totalAmount;
};

struct PaymentSummary {
    Decimal totalPaid;
    Decimal outstandingAmount;
};

struct ResidentPaymentProfile {
    string residentId;
    string fullName;
    string icNumber;
    ResidentStatus status;
};

struct QuarterDisplay {
    string quarterName;
    string unitCode;
};

struct PaymentHistoryRow {
    Date paymentDate;
    string receiptNo;
    Decimal amount;
    string description;
};

struct RecordStringPaymentDetail {
    string key;
    string value;
};

struct UnitOccupancy {
    string unitCode;
    string quarterName;
    bool active;
};

struct ArrearsSummary {
    string residentId;
    Decimal outstandingAmount;
};

struct QueryFilter {
    string keyword;
};

// =====================================================
// Domain Entity: Transaction
// =====================================================

class Transaction {
private:
    string transactionNo;
    string residentId;
    string paymentId;
    Decimal debitAmount;
    Decimal creditAmount;

public:
    Transaction() {
        transactionNo = "";
        residentId = "";
        paymentId = "";
        debitAmount = 0.0;
        creditAmount = 0.0;
    }

    Transaction(
        string transactionNo,
        string residentId,
        string paymentId,
        Decimal debitAmount,
        Decimal creditAmount
    ) {
        this->transactionNo = transactionNo;
        this->residentId = residentId;
        this->paymentId = paymentId;
        this->debitAmount = debitAmount;
        this->creditAmount = creditAmount;
    }

    string getTransactionNo() const {
        return transactionNo;
    }

    Decimal getNetAmount() const {
        return debitAmount - creditAmount;
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
    string phone;
    string email;
    string position;
    string department;
    string serviceLevel;
    ResidentStatus status;
    Decimal outstandingBalance;
    UnitOccupancy currentOccupancy;

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
        status = ResidentStatus::DATA_TIDAK_LENGKAP;
        outstandingBalance = 0.0;
        currentOccupancy = { "", "", false };
    }

    Resident(
        string id,
        string fullName,
        string icNumber,
        ResidentStatus status,
        Decimal outstandingBalance,
        UnitOccupancy occupancy
    ) {
        this->id = id;
        this->fullName = fullName;
        this->icNumber = icNumber;
        this->phone = "";
        this->email = "";
        this->position = "";
        this->department = "";
        this->serviceLevel = "";
        this->status = status;
        this->outstandingBalance = outstandingBalance;
        this->currentOccupancy = occupancy;
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

    ResidentStatus getStatus() const {
        return status;
    }

    UnitOccupancy getCurrentOccupancy() const {
        return currentOccupancy;
    }

    Decimal getOutstandingBalance() const {
        return outstandingBalance;
    }

    Resident updateDetails(string newPhone, string newEmail) {
        phone = newPhone;
        email = newEmail;
        return *this;
    }

    Resident changeStatus(ResidentStatus newStatus, string description) {
        status = newStatus;
        cout << "Resident status changed: " << description << endl;
        return *this;
    }

    int calculateAge(Date referenceDate) const {
        if (icNumber.length() < 2) {
            return 0;
        }

        int yearPrefix = stoi(icNumber.substr(0, 2));
        int birthYear = yearPrefix <= 30 ? 2000 + yearPrefix : 1900 + yearPrefix;
        int referenceYear = stoi(referenceDate.substr(0, 4));

        return referenceYear - birthYear;
    }

    bool isEligibleForBilling() const {
        return status == ResidentStatus::AKTIF ||
               status == ResidentStatus::PENCEN_MENDATANG;
    }

    bool isDataIncomplete() const {
        return fullName.empty() || icNumber.empty() || !currentOccupancy.active;
    }

    ArrearsSummary getArrearsSummary() const {
        return { id, outstandingBalance };
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

    Payment updateDetails(
        Date paymentDate,
        string receiptNo,
        Decimal amount,
        string description
    ) {
        this->paymentDate = paymentDate;
        this->receiptNo = receiptNo;
        this->amount = amount;
        this->description = description;
        return *this;
    }

    Transaction toTransaction(string transactionNo) {
        return Transaction(
            transactionNo,
            residentId,
            id,
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

    string getId() const {
        return id;
    }

    string getResidentId() const {
        return residentId;
    }

    Date getPaymentDate() const {
        return paymentDate;
    }

    string getReceiptNo() const {
        return receiptNo;
    }

    string getDescription() const {
        return description;
    }
};

// =====================================================
// DTO: PaymentRow
// =====================================================

class PaymentRow {
public:
    string id;
    string residentId;
    string name;
    string ic;
    string quarters;
    string unit;
    string arrears;
    string amount;
    PaymentTone tone;
    PaymentStatusFilter paymentStatus;
    string searchText;

    bool matchesSearch(string keyword) const {
        if (keyword.empty()) {
            return true;
        }

        string combined = name + " " + ic + " " + quarters + " " + unit + " " + searchText;
        return combined.find(keyword) != string::npos;
    }

    bool matchesStatus(PaymentStatusFilter status) const {
        return paymentStatus == status;
    }
};

// =====================================================
// DTO: PaymentDetail
// =====================================================

class PaymentDetail {
public:
    string id;
    ResidentPaymentProfile resident;
    QuarterDisplay quarters;
    PaymentSummary payment;
    bool historyLoaded;
    vector<PaymentHistoryRow> history;

    bool hasHistory() const {
        return historyLoaded && !history.empty();
    }

    Decimal getCurrentOutstandingAmount() const {
        return payment.outstandingAmount;
    }
};

// =====================================================
// DTO: PaymentPage
// =====================================================

class PaymentPage {
public:
    vector<PaymentRow> rows;
    vector<PaymentExportRow> exportRows;
    vector<RecordStringPaymentDetail> stats;
    vector<pair<string, PaymentDetail>> details;

    int getRecordCount() const {
        return static_cast<int>(rows.size());
    }

    vector<PaymentExportRow> getExportableRows() const {
        return exportRows;
    }
};

// =====================================================
// Repository Interface
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
// Repository: PaymentRepository
// =====================================================

class PaymentRepository : public IRepository<Payment> {
private:
    vector<Resident> residents;
    vector<Payment> payments;

public:
    PaymentRepository() {
        residents.push_back(Resident(
            "RES_001",
            "Ali Bin Abu",
            "660101011111",
            ResidentStatus::AKTIF,
            300.00,
            { "A-01", "Class A Quarters", true }
        ));

        residents.push_back(Resident(
            "RES_002",
            "Siti Binti Ahmad",
            "900505015555",
            ResidentStatus::AKTIF,
            250.00,
            { "B-02", "Class B Quarters", true }
        ));

        residents.push_back(Resident(
            "RES_003",
            "Kumar A/L Ravi",
            "591212015555",
            ResidentStatus::DATA_TIDAK_LENGKAP,
            200.00,
            { "", "", false }
        ));

        payments.push_back(Payment(
            "PAY_001",
            "RES_001",
            "2026-06-05",
            "RCP-001",
            300.00,
            "Monthly salary deduction payment."
        ));

        payments.push_back(Payment(
            "PAY_002",
            "RES_002",
            "2026-06-06",
            "RCP-002",
            400.00,
            "Overpaid amount detected."
        ));
    }

    Payment findById(string id) override {
        for (Payment payment : payments) {
            if (payment.getId() == id) {
                return payment;
            }
        }

        return Payment();
    }

    vector<Payment> findMany(QueryFilter filter) override {
        vector<Payment> result;

        for (Payment payment : payments) {
            if (filter.keyword.empty() ||
                payment.getReceiptNo().find(filter.keyword) != string::npos ||
                payment.getResidentId().find(filter.keyword) != string::npos) {
                result.push_back(payment);
            }
        }

        return result;
    }

    Payment create(Payment input) override {
        payments.push_back(input);
        return input;
    }

    Payment update(string id, Payment input) override {
        for (int i = 0; i < static_cast<int>(payments.size()); i++) {
            if (payments[i].getId() == id) {
                payments[i] = input;
                return payments[i];
            }
        }

        return Payment();
    }

    bool remove(string id) override {
        for (int i = 0; i < static_cast<int>(payments.size()); i++) {
            if (payments[i].getId() == id) {
                payments.erase(payments.begin() + i);
                return true;
            }
        }

        return false;
    }

    vector<Resident> getResidents() {
        return residents;
    }

    Resident findResidentById(string residentId) {
        for (Resident resident : residents) {
            if (resident.getId() == residentId) {
                return resident;
            }
        }

        return Resident();
    }

    vector<PaymentRow> findPaymentListRows(Date paymentMonth) {
        vector<PaymentRow> rows;

        for (Resident resident : residents) {
            Decimal paidAmount = sumResidentPaymentsForMonth(
                resident.getId(),
                paymentMonth + "-01",
                paymentMonth + "-31"
            );

            UnitOccupancy occupancy = findCurrentOccupancyByResident(resident.getId());
            ArrearsSummary arrears = findArrearsByResident(resident.getId());

            PaymentStatusFilter status = PaymentStatusFilter::KURANG;
            PaymentTone tone = PaymentTone::RED;

            Decimal remaining = arrears.outstandingAmount - paidAmount;

            if (resident.getStatus() == ResidentStatus::DATA_TIDAK_LENGKAP || !occupancy.active) {
                status = PaymentStatusFilter::TIDAK_LENGKAP;
                tone = PaymentTone::PURPLE;
            } else if (remaining == 0) {
                status = PaymentStatusFilter::CUKUP;
                tone = PaymentTone::GREEN;
            } else if (remaining < 0) {
                status = PaymentStatusFilter::LEBIH;
                tone = PaymentTone::BLUE;
            } else {
                status = PaymentStatusFilter::KURANG;
                tone = PaymentTone::RED;
            }

            rows.push_back({
                "ROW_" + resident.getId(),
                resident.getId(),
                resident.getFullName(),
                resident.getIcNumber(),
                occupancy.quarterName,
                occupancy.unitCode,
                to_string(arrears.outstandingAmount),
                to_string(paidAmount),
                tone,
                status,
                resident.getFullName() + " " + resident.getIcNumber()
            });
        }

        return rows;
    }

    PaymentStatsQueryRow findPaymentStats(Date paymentMonth) {
        Decimal totalAmount = 0.0;

        for (Payment payment : payments) {
            if (payment.getPaymentDate().substr(0, 7) == paymentMonth) {
                totalAmount += payment.getAmount();
            }
        }

        return {
            static_cast<int>(residents.size()),
            static_cast<int>(payments.size()),
            totalAmount
        };
    }

    PaymentDetail findPaymentDetailsById(string paymentId, bool includeHistory) {
        Payment payment = findById(paymentId);

        if (payment.getId().empty()) {
            return PaymentDetail();
        }

        Resident resident = findResidentById(payment.getResidentId());
        UnitOccupancy occupancy = findCurrentOccupancyByResident(resident.getId());
        ArrearsSummary arrears = findArrearsByResident(resident.getId());

        PaymentDetail detail;
        detail.id = payment.getId();
        detail.resident = {
            resident.getId(),
            resident.getFullName(),
            resident.getIcNumber(),
            resident.getStatus()
        };
        detail.quarters = {
            occupancy.quarterName,
            occupancy.unitCode
        };
        detail.payment = {
            payment.getAmount(),
            arrears.outstandingAmount - payment.getAmount()
        };
        detail.historyLoaded = includeHistory;

        if (includeHistory) {
            for (Payment item : payments) {
                if (item.belongsToResident(resident.getId())) {
                    detail.history.push_back({
                        item.getPaymentDate(),
                        item.getReceiptNo(),
                        item.getAmount(),
                        item.getDescription()
                    });
                }
            }
        }

        return detail;
    }

    Decimal sumResidentPaymentsForMonth(
        string residentId,
        Date monthStartDate,
        Date nextMonthStartDate
    ) {
        Decimal total = 0.0;

        for (Payment payment : payments) {
            if (payment.belongsToResident(residentId) &&
                payment.isInMonth(monthStartDate, nextMonthStartDate)) {
                total += payment.getAmount();
            }
        }

        return total;
    }

    UnitOccupancy findCurrentOccupancyByResident(string residentId) {
        Resident resident = findResidentById(residentId);
        return resident.getCurrentOccupancy();
    }

    ArrearsSummary findArrearsByResident(string residentId) {
        Resident resident = findResidentById(residentId);
        return resident.getArrearsSummary();
    }

    Payment createPayment(PaymentCreateInput input) {
        string newId = "PAY_" + to_string(payments.size() + 1);

        Payment payment(
            newId,
            input.residentId,
            input.paymentDate,
            input.receiptNo,
            input.amount,
            input.description
        );

        return create(payment);
    }
};

// =====================================================
// Mapper Interface and Mapper
// =====================================================

template <typename T, typename D>
class EntityMapper {
public:
    virtual D toDto(T entity) = 0;
    virtual vector<D> toDtoList(vector<T> entities) = 0;
    virtual ~EntityMapper() {}
};

class PaymentMapper : public EntityMapper<Payment, PaymentDetail> {
public:
    PaymentDetail toDto(Payment entity) override {
        PaymentDetail detail;
        detail.id = entity.getId();
        detail.payment = {
            entity.getAmount(),
            0.0
        };
        detail.historyLoaded = false;
        return detail;
    }

    vector<PaymentDetail> toDtoList(vector<Payment> entities) override {
        vector<PaymentDetail> result;

        for (Payment payment : entities) {
            result.push_back(toDto(payment));
        }

        return result;
    }
};

// =====================================================
// Behavioral Pattern: Strategy
// Interface: IPaymentStatusStrategy
// =====================================================

class IPaymentStatusStrategy {
public:
    virtual PaymentStatusFilter resolveStatus(
        ResidentStatus residentStatus,
        Decimal arrearsAmount
    ) = 0;

    virtual string resolveLabel(PaymentStatusFilter status) = 0;

    virtual PaymentTone resolveTone(PaymentStatusFilter status) = 0;

    virtual bool supports(
        ResidentStatus residentStatus,
        Decimal arrearsAmount
    ) = 0;

    virtual ~IPaymentStatusStrategy() {}
};

// =====================================================
// Concrete Strategy: CompletePaymentStatusStrategy
// =====================================================

class CompletePaymentStatusStrategy : public IPaymentStatusStrategy {
public:
    PaymentStatusFilter resolveStatus(
        ResidentStatus residentStatus,
        Decimal arrearsAmount
    ) override {
        return PaymentStatusFilter::CUKUP;
    }

    string resolveLabel(PaymentStatusFilter status) override {
        return "Payment is complete.";
    }

    PaymentTone resolveTone(PaymentStatusFilter status) override {
        return PaymentTone::GREEN;
    }

    bool supports(
        ResidentStatus residentStatus,
        Decimal arrearsAmount
    ) override {
        return residentStatus != ResidentStatus::DATA_TIDAK_LENGKAP &&
               arrearsAmount == 0;
    }
};

// =====================================================
// Concrete Strategy: OverpaidPaymentStatusStrategy
// =====================================================

class OverpaidPaymentStatusStrategy : public IPaymentStatusStrategy {
public:
    PaymentStatusFilter resolveStatus(
        ResidentStatus residentStatus,
        Decimal arrearsAmount
    ) override {
        return PaymentStatusFilter::LEBIH;
    }

    string resolveLabel(PaymentStatusFilter status) override {
        return "Payment is overpaid.";
    }

    PaymentTone resolveTone(PaymentStatusFilter status) override {
        return PaymentTone::BLUE;
    }

    bool supports(
        ResidentStatus residentStatus,
        Decimal arrearsAmount
    ) override {
        return residentStatus != ResidentStatus::DATA_TIDAK_LENGKAP &&
               arrearsAmount < 0;
    }
};

// =====================================================
// Concrete Strategy: UnderpaidPaymentStatusStrategy
// =====================================================

class UnderpaidPaymentStatusStrategy : public IPaymentStatusStrategy {
public:
    PaymentStatusFilter resolveStatus(
        ResidentStatus residentStatus,
        Decimal arrearsAmount
    ) override {
        return PaymentStatusFilter::KURANG;
    }

    string resolveLabel(PaymentStatusFilter status) override {
        return "Payment is underpaid.";
    }

    PaymentTone resolveTone(PaymentStatusFilter status) override {
        return PaymentTone::RED;
    }

    bool supports(
        ResidentStatus residentStatus,
        Decimal arrearsAmount
    ) override {
        return residentStatus != ResidentStatus::DATA_TIDAK_LENGKAP &&
               arrearsAmount > 0;
    }
};

// =====================================================
// Concrete Strategy: IncompleteResidentPaymentStatusStrategy
// =====================================================

class IncompleteResidentPaymentStatusStrategy : public IPaymentStatusStrategy {
public:
    PaymentStatusFilter resolveStatus(
        ResidentStatus residentStatus,
        Decimal arrearsAmount
    ) override {
        return PaymentStatusFilter::TIDAK_LENGKAP;
    }

    string resolveLabel(PaymentStatusFilter status) override {
        return "Resident data is incomplete.";
    }

    PaymentTone resolveTone(PaymentStatusFilter status) override {
        return PaymentTone::PURPLE;
    }

    bool supports(
        ResidentStatus residentStatus,
        Decimal arrearsAmount
    ) override {
        return residentStatus == ResidentStatus::DATA_TIDAK_LENGKAP;
    }
};

// =====================================================
// Behavioral Pattern: Chain of Responsibility
// Abstract Class: PaymentFilterHandler
// =====================================================

class PaymentFilterHandler {
protected:
    shared_ptr<PaymentFilterHandler> next;

    virtual vector<PaymentRow> applyCurrent(
        vector<PaymentRow> rows,
        PaymentFilters filters
    ) = 0;

public:
    shared_ptr<PaymentFilterHandler> setNext(
        shared_ptr<PaymentFilterHandler> handler
    ) {
        next = handler;
        return handler;
    }

    vector<PaymentRow> filter(
        vector<PaymentRow> rows,
        PaymentFilters filters
    ) {
        vector<PaymentRow> currentRows = applyCurrent(rows, filters);

        if (next != nullptr) {
            return next->filter(currentRows, filters);
        }

        return currentRows;
    }

    virtual ~PaymentFilterHandler() {}
};

// =====================================================
// Concrete Handler: PaymentSearchFilterHandler
// =====================================================

class PaymentSearchFilterHandler : public PaymentFilterHandler {
protected:
    vector<PaymentRow> applyCurrent(
        vector<PaymentRow> rows,
        PaymentFilters filters
    ) override {
        if (filters.search.empty()) {
            return rows;
        }

        vector<PaymentRow> result;

        for (PaymentRow row : rows) {
            if (row.matchesSearch(filters.search)) {
                result.push_back(row);
            }
        }

        return result;
    }
};

// =====================================================
// Concrete Handler: PaymentStatusFilterHandler
// =====================================================

class PaymentStatusFilterHandler : public PaymentFilterHandler {
protected:
    vector<PaymentRow> applyCurrent(
        vector<PaymentRow> rows,
        PaymentFilters filters
    ) override {
        if (filters.statuses.empty()) {
            return rows;
        }

        vector<PaymentRow> result;

        for (PaymentRow row : rows) {
            for (PaymentStatusFilter status : filters.statuses) {
                if (row.matchesStatus(status)) {
                    result.push_back(row);
                    break;
                }
            }
        }

        return result;
    }
};

// =====================================================
// Service Layer: PaymentService
// =====================================================

class PaymentService {
private:
    PaymentRepository& paymentRepository;
    vector<shared_ptr<IPaymentStatusStrategy>> statusStrategies;

    shared_ptr<PaymentFilterHandler> buildFilterChain() {
        shared_ptr<PaymentFilterHandler> searchHandler =
            make_shared<PaymentSearchFilterHandler>();

        shared_ptr<PaymentFilterHandler> statusHandler =
            make_shared<PaymentStatusFilterHandler>();

        searchHandler->setNext(statusHandler);
        return searchHandler;
    }

public:
    PaymentService(PaymentRepository& repository)
        : paymentRepository(repository) {
        statusStrategies.push_back(make_shared<IncompleteResidentPaymentStatusStrategy>());
        statusStrategies.push_back(make_shared<CompletePaymentStatusStrategy>());
        statusStrategies.push_back(make_shared<OverpaidPaymentStatusStrategy>());
        statusStrategies.push_back(make_shared<UnderpaidPaymentStatusStrategy>());
    }

    vector<PaymentRow> getPaymentRows(
        Date paymentMonth,
        PaymentFilters filters
    ) {
        vector<PaymentRow> rows =
            paymentRepository.findPaymentListRows(paymentMonth);

        shared_ptr<PaymentFilterHandler> filterChain = buildFilterChain();
        return filterChain->filter(rows, filters);
    }

    PaymentStats getPaymentStats(Date paymentMonth) {
        vector<PaymentRow> rows =
            paymentRepository.findPaymentListRows(paymentMonth);

        PaymentStats stats = { 0, 0, 0, 0 };

        for (PaymentRow row : rows) {
            if (row.paymentStatus == PaymentStatusFilter::KURANG) {
                stats.kurangCount++;
            } else if (row.paymentStatus == PaymentStatusFilter::LEBIH) {
                stats.lebihCount++;
            } else if (row.paymentStatus == PaymentStatusFilter::TIDAK_LENGKAP) {
                stats.tidakLengkapCount++;
            } else if (row.paymentStatus == PaymentStatusFilter::CUKUP) {
                stats.cukupCount++;
            }
        }

        return stats;
    }

    pair<vector<RecordStringPaymentDetail>, PaymentDetail> getPaymentDetails(
        string paymentId,
        bool includeHistory
    ) {
        PaymentDetail detail =
            paymentRepository.findPaymentDetailsById(paymentId, includeHistory);

        vector<RecordStringPaymentDetail> records = {
            { "Payment ID", detail.id },
            { "Resident", detail.resident.fullName },
            { "Outstanding", to_string(detail.getCurrentOutstandingAmount()) }
        };

        return { records, detail };
    }

    Decimal getCurrentMonthPaidAmount(
        string residentId,
        Date month
    ) {
        return paymentRepository.sumResidentPaymentsForMonth(
            residentId,
            month + "-01",
            month + "-31"
        );
    }

    PaymentStatusFilter classifyPaymentStatus(
        ResidentStatus residentStatus,
        Decimal arrearsAmount
    ) {
        for (shared_ptr<IPaymentStatusStrategy> strategy : statusStrategies) {
            if (strategy->supports(residentStatus, arrearsAmount)) {
                return strategy->resolveStatus(residentStatus, arrearsAmount);
            }
        }

        return PaymentStatusFilter::KURANG;
    }

    vector<PaymentExportRow> buildExportRows(vector<PaymentRow> rows) {
        vector<PaymentExportRow> exportRows;

        for (PaymentRow row : rows) {
            exportRows.push_back({
                row.name,
                row.ic,
                row.unit,
                paymentStatusToString(row.paymentStatus),
                row.amount
            });
        }

        return exportRows;
    }

    Payment createManualPayment(PaymentCreateInput input, AdminProfile actor) {
        cout << "Manual payment created by admin: "
             << actor.username << endl;

        return paymentRepository.createPayment(input);
    }
};

// =====================================================
// Structural Pattern: Facade
// Class: PaymentFacade
// =====================================================

class PaymentFacade {
private:
    PaymentService& paymentService;

public:
    PaymentFacade(PaymentService& service)
        : paymentService(service) {
    }

    PaymentPage loadPageData(Date paymentMonth) {
        PaymentFilters filters = { "", {} };
        vector<PaymentRow> rows =
            paymentService.getPaymentRows(paymentMonth, filters);

        PaymentStats stats =
            paymentService.getPaymentStats(paymentMonth);

        vector<RecordStringPaymentDetail> statRecords = {
            { "KURANG", to_string(stats.kurangCount) },
            { "LEBIH", to_string(stats.lebihCount) },
            { "TIDAK_LENGKAP", to_string(stats.tidakLengkapCount) },
            { "CUKUP", to_string(stats.cukupCount) }
        };

        PaymentPage page;
        page.rows = rows;
        page.exportRows = paymentService.buildExportRows(rows);
        page.stats = statRecords;

        return page;
    }

    pair<vector<RecordStringPaymentDetail>, PaymentDetail> loadPaymentDetails(
        string paymentId,
        bool includeHistory
    ) {
        return paymentService.getPaymentDetails(paymentId, includeHistory);
    }

    PaymentDetail addManualPayment(
        PaymentCreateInput input,
        string actorId
    ) {
        AdminProfile actor = { actorId, "paymentAdmin" };
        Payment payment = paymentService.createManualPayment(input, actor);

        PaymentDetail detail;
        detail.id = payment.getId();
        detail.payment = { payment.getAmount(), 0.0 };
        detail.historyLoaded = false;

        return detail;
    }

    PaymentPage reloadAfterPaymentChange(Date paymentMonth) {
        return loadPageData(paymentMonth);
    }
};

// =====================================================
// Controller Layer: PaymentController
// =====================================================

class PaymentController {
private:
    PaymentFacade& paymentFacade;
    PaymentService& paymentService;

public:
    PaymentController(
        PaymentFacade& facade,
        PaymentService& service
    ) : paymentFacade(facade),
        paymentService(service) {
    }

    ApiResponse<PaymentPage> getPaymentList(
        PaymentListRequest request
    ) {
        PaymentPage page = paymentFacade.loadPageData(request.paymentMonth);

        PaymentFilters filters = {
            request.search,
            request.statusFilters
        };

        page.rows = paymentService.getPaymentRows(
            request.paymentMonth,
            filters
        );
        page.exportRows = paymentService.buildExportRows(page.rows);

        return {
            true,
            "Payment page loaded successfully.",
            page
        };
    }

    ApiResponse<PaymentDetail> getPaymentDetail(
        PaymentDetailRequest request
    ) {
        pair<vector<RecordStringPaymentDetail>, PaymentDetail> result =
            paymentFacade.loadPaymentDetails(
                request.paymentId,
                request.includeHistory
            );

        if (result.second.id.empty()) {
            return {
                false,
                "Payment detail not found.",
                PaymentDetail()
            };
        }

        return {
            true,
            "Payment detail retrieved successfully.",
            result.second
        };
    }

    ApiResponse<PaymentDetail> createManualPayment(
        ManualPaymentRequest request
    ) {
        PaymentCreateInput input = {
            request.residentId,
            request.paymentDate,
            request.receiptNo,
            request.amount,
            request.description
        };

        PaymentDetail detail =
            paymentFacade.addManualPayment(input, request.actor.id);

        return {
            true,
            "Manual payment created successfully.",
            detail
        };
    }
};

// =====================================================
// Main Function
// =====================================================

int main() {
    cout << fixed << setprecision(2);

    PaymentRepository paymentRepository;
    PaymentService paymentService(paymentRepository);
    PaymentFacade paymentFacade(paymentService);
    PaymentController paymentController(paymentFacade, paymentService);

    PaymentListRequest listRequest = {
        "2026-06",
        "",
        {}
    };

    ApiResponse<PaymentPage> listResponse =
        paymentController.getPaymentList(listRequest);

    cout << "===== Payment Page =====" << endl;
    cout << listResponse.message << endl;
    cout << "Record Count: " << listResponse.data.getRecordCount() << endl;

    for (PaymentRow row : listResponse.data.rows) {
        cout << row.name
             << " | IC: " << row.ic
             << " | Unit: " << row.unit
             << " | Paid: RM " << row.amount
             << " | Arrears: RM " << row.arrears
             << " | Status: " << paymentStatusToString(row.paymentStatus)
             << " | Tone: " << paymentToneToString(row.tone)
             << endl;
    }

    PaymentDetailRequest detailRequest = {
        "PAY_001",
        true
    };

    ApiResponse<PaymentDetail> detailResponse =
        paymentController.getPaymentDetail(detailRequest);

    cout << "\n===== Payment Detail =====" << endl;
    cout << detailResponse.message << endl;

    if (detailResponse.success) {
        cout << "Payment ID: " << detailResponse.data.id << endl;
        cout << "Resident: " << detailResponse.data.resident.fullName << endl;
        cout << "Quarter: " << detailResponse.data.quarters.quarterName << endl;
        cout << "Unit: " << detailResponse.data.quarters.unitCode << endl;
        cout << "Current Outstanding: RM "
             << detailResponse.data.getCurrentOutstandingAmount() << endl;
        cout << "History Loaded: "
             << (detailResponse.data.hasHistory() ? "Yes" : "No") << endl;
    }

    ManualPaymentRequest manualRequest = {
        "RES_003",
        "2026-06-20",
        "RCP-003",
        200.00,
        "Manual payment added after review.",
        { "ADMIN_001", "paymentAdmin" }
    };

    ApiResponse<PaymentDetail> manualResponse =
        paymentController.createManualPayment(manualRequest);

    cout << "\n===== Manual Payment Result =====" << endl;
    cout << manualResponse.message << endl;
    cout << "Created Payment ID: " << manualResponse.data.id << endl;
    cout << "Amount: RM " << manualResponse.data.payment.totalPaid << endl;

    PaymentListRequest filteredRequest = {
        "2026-06",
        "Ali",
        { PaymentStatusFilter::CUKUP }
    };

    ApiResponse<PaymentPage> filteredResponse =
        paymentController.getPaymentList(filteredRequest);

    cout << "\n===== Filtered Payment Page =====" << endl;
    cout << "Record Count: " << filteredResponse.data.getRecordCount() << endl;

    for (PaymentRow row : filteredResponse.data.rows) {
        cout << row.name
             << " | Status: " << paymentStatusToString(row.paymentStatus)
             << endl;
    }

    return 0;
}
