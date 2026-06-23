#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>

using namespace std;

// =====================================================
// Enumerations
// =====================================================

enum class DashboardMetricType {
    COLLECTION,
    UNIT_STATUS,
    ARREARS,
    PENDING_VERIFICATION,
    QUARTER_ARREARS
};

string metricTypeToString(DashboardMetricType type) {
    switch (type) {
        case DashboardMetricType::COLLECTION:
            return "COLLECTION";
        case DashboardMetricType::UNIT_STATUS:
            return "UNIT_STATUS";
        case DashboardMetricType::ARREARS:
            return "ARREARS";
        case DashboardMetricType::PENDING_VERIFICATION:
            return "PENDING_VERIFICATION";
        case DashboardMetricType::QUARTER_ARREARS:
            return "QUARTER_ARREARS";
        default:
            return "UNKNOWN";
    }
}

// =====================================================
// DTOs and Interfaces
// =====================================================

struct MetricDelta {
    double differenceAmount;
    double differencePercent;
    bool isImproved;
};

struct DashboardMetric {
    DashboardMetricType type;
    string label;
    double currentValue;
    double comparisonValue;
    MetricDelta delta;
    string formattedValue;
};

struct CollectionSummary {
    double currentMonthTotal;
    double previousMonthTotal;
    double differenceAmount;
    double differencePercent;

    bool isImproved() const {
        return currentMonthTotal >= previousMonthTotal;
    }

    string formatDifferenceLabel() const {
        string direction = isImproved() ? "increase" : "decrease";
        return direction + " of " + to_string(differencePercent) + "% compared to previous month";
    }
};

struct UnitOccupancySummary {
    int occupiedCount;
    int vacantCount;
    int totalUnits;

    double getOccupancyRate() const {
        if (totalUnits == 0) {
            return 0.0;
        }

        return (static_cast<double>(occupiedCount) / totalUnits) * 100.0;
    }
};

struct ArrearsOverview {
    double totalUnpaidArrears;
    int residentCount;

    bool hasOutstandingArrears() const {
        return totalUnpaidArrears > 0;
    }
};

struct PendingVerificationSummary {
    int residentDraftCount;
    int paymentDraftCount;
    int arrearsSummaryDraftCount;
    int quarterCategoryDraftCount;
    int unitDraftCount;
    int totalDraftCount;

    bool hasPendingDrafts() const {
        return totalDraftCount > 0;
    }
};

struct QuarterArrearsAnalysis {
    string categoryId;
    string categoryName;
    string address;
    int residentCount;
    double totalArrearsAmount;
    double averageArrearsAmount;

    bool isHighestRisk(double currentHighestAmount) const {
        return totalArrearsAmount >= currentHighestAmount;
    }
};

struct DashboardSummary {
    CollectionSummary collection;
    UnitOccupancySummary units;
    ArrearsOverview arrears;
    PendingVerificationSummary pendingVerification;
    vector<QuarterArrearsAnalysis> quarterArrears;

    bool hasPendingAction() const {
        return pendingVerification.hasPendingDrafts() || arrears.hasOutstandingArrears();
    }

    QuarterArrearsAnalysis getHighestRiskQuarter() const {
        if (quarterArrears.empty()) {
            return { "", "No Data", "-", 0, 0.0, 0.0 };
        }

        QuarterArrearsAnalysis highest = quarterArrears[0];

        for (QuarterArrearsAnalysis item : quarterArrears) {
            if (item.totalArrearsAmount > highest.totalArrearsAmount) {
                highest = item;
            }
        }

        return highest;
    }
};

struct DashboardSummaryRequest {
    string referenceDate;
    string actorId;
};

template <typename T>
struct ApiResponse {
    bool success;
    string message;
    T data;
};

struct QueryFilter {
    string keyword;
};

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
// Repository Layer
// Class: DashboardRepository
// =====================================================
// In the real system, this class would query the database.
// In this C++ demonstration, fixed values are used to simulate dashboard data.

class DashboardRepository : public IRepository<DashboardSummary> {
public:
    DashboardSummary findById(string id) override {
        return DashboardSummary();
    }

    vector<DashboardSummary> findMany(QueryFilter filter) override {
        return {};
    }

    DashboardSummary create(DashboardSummary input) override {
        return input;
    }

    DashboardSummary update(string id, DashboardSummary input) override {
        return input;
    }

    bool remove(string id) override {
        return true;
    }

    double sumPaymentsBetween(string startDate, string endDate) {
        if (startDate == "2026-06-01") {
            return 25800.50;
        }

        if (startDate == "2026-05-01") {
            return 23640.00;
        }

        return 0.0;
    }

    int countUnitsByStatus(string unitStatus) {
        if (unitStatus == "OCCUPIED") {
            return 82;
        }

        if (unitStatus == "VACANT") {
            return 18;
        }

        return 0;
    }

    double sumPositiveArrears() {
        return 7200.00;
    }

    int countResidentsWithArrears() {
        return 12;
    }

    int countResidentDrafts() {
        return 4;
    }

    int countPaymentDrafts() {
        return 7;
    }

    int countArrearsSummaryDrafts() {
        return 2;
    }

    int countQuarterCategoryDrafts() {
        return 1;
    }

    int countUnitDrafts() {
        return 3;
    }

    vector<QuarterArrearsAnalysis> findArrearsGroupedByQuarterCategory() {
        return {
            { "QCAT_001", "Class A Quarters", "Block A", 4, 3200.00, 800.00 },
            { "QCAT_002", "Class B Quarters", "Block B", 5, 2500.00, 500.00 },
            { "QCAT_003", "Class C Quarters", "Block C", 3, 1500.00, 500.00 }
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
// Mapper
// Class: DashboardMapper
// =====================================================

class DashboardMapper : public EntityMapper<DashboardSummary, DashboardSummary> {
public:
    DashboardSummary toDto(DashboardSummary entity) override {
        return entity;
    }

    vector<DashboardSummary> toDtoList(vector<DashboardSummary> entities) override {
        return entities;
    }
};

// =====================================================
// Behavioral Pattern: Template Method
// Abstract Class: DashboardMetricTemplate
// =====================================================
// This class defines the fixed steps for building a dashboard metric.
// Subclasses only customize how the current value and comparison value are fetched.

class DashboardMetricTemplate {
protected:
    DashboardRepository& dashboardRepository;

    virtual DashboardMetricType getMetricType() = 0;

    virtual string getMetricLabel() = 0;

    virtual double fetchCurrentValue(string referenceDate) = 0;

    virtual double fetchComparisonValue(string referenceDate) = 0;

    virtual MetricDelta calculateDelta(double currentValue, double comparisonValue) {
        double differenceAmount = currentValue - comparisonValue;
        double differencePercent = 0.0;

        if (comparisonValue != 0) {
            differencePercent = (differenceAmount / comparisonValue) * 100.0;
        }

        return {
            differenceAmount,
            differencePercent,
            differenceAmount >= 0
        };
    }

    virtual DashboardMetric formatMetric(
        double currentValue,
        double comparisonValue,
        MetricDelta delta
    ) {
        return {
            getMetricType(),
            getMetricLabel(),
            currentValue,
            comparisonValue,
            delta,
            getMetricLabel() + ": " + to_string(currentValue)
        };
    }

public:
    DashboardMetricTemplate(DashboardRepository& repository)
        : dashboardRepository(repository) {
    }

    DashboardMetric build(string referenceDate) {
        double currentValue = fetchCurrentValue(referenceDate);
        double comparisonValue = fetchComparisonValue(referenceDate);
        MetricDelta delta = calculateDelta(currentValue, comparisonValue);

        return formatMetric(currentValue, comparisonValue, delta);
    }

    virtual ~DashboardMetricTemplate() {}
};

// =====================================================
// Behavioral Pattern: Strategy
// Interface: IDashboardMetricStrategy
// =====================================================

class IDashboardMetricStrategy {
public:
    virtual DashboardMetric calculate(string referenceDate) = 0;
    virtual bool supports(DashboardMetricType metricType) = 0;
    virtual ~IDashboardMetricStrategy() {}
};

// =====================================================
// Concrete Strategy: CollectionMetricStrategy
// =====================================================

class CollectionMetricStrategy : public DashboardMetricTemplate, public IDashboardMetricStrategy {
protected:
    DashboardMetricType getMetricType() override {
        return DashboardMetricType::COLLECTION;
    }

    string getMetricLabel() override {
        return "Monthly Collection";
    }

    double fetchCurrentValue(string referenceDate) override {
        return dashboardRepository.sumPaymentsBetween("2026-06-01", "2026-07-01");
    }

    double fetchComparisonValue(string referenceDate) override {
        return dashboardRepository.sumPaymentsBetween("2026-05-01", "2026-06-01");
    }

public:
    CollectionMetricStrategy(DashboardRepository& repository)
        : DashboardMetricTemplate(repository) {
    }

    DashboardMetric calculate(string referenceDate) override {
        return build(referenceDate);
    }

    bool supports(DashboardMetricType metricType) override {
        return metricType == DashboardMetricType::COLLECTION;
    }
};

// =====================================================
// Concrete Strategy: UnitStatusMetricStrategy
// =====================================================

class UnitStatusMetricStrategy : public DashboardMetricTemplate, public IDashboardMetricStrategy {
protected:
    DashboardMetricType getMetricType() override {
        return DashboardMetricType::UNIT_STATUS;
    }

    string getMetricLabel() override {
        return "Occupied Units";
    }

    double fetchCurrentValue(string referenceDate) override {
        return dashboardRepository.countUnitsByStatus("OCCUPIED");
    }

    double fetchComparisonValue(string referenceDate) override {
        return dashboardRepository.countUnitsByStatus("VACANT");
    }

public:
    UnitStatusMetricStrategy(DashboardRepository& repository)
        : DashboardMetricTemplate(repository) {
    }

    DashboardMetric calculate(string referenceDate) override {
        return build(referenceDate);
    }

    bool supports(DashboardMetricType metricType) override {
        return metricType == DashboardMetricType::UNIT_STATUS;
    }
};

// =====================================================
// Concrete Strategy: ArrearsMetricStrategy
// =====================================================

class ArrearsMetricStrategy : public DashboardMetricTemplate, public IDashboardMetricStrategy {
protected:
    DashboardMetricType getMetricType() override {
        return DashboardMetricType::ARREARS;
    }

    string getMetricLabel() override {
        return "Total Arrears";
    }

    double fetchCurrentValue(string referenceDate) override {
        return dashboardRepository.sumPositiveArrears();
    }

    double fetchComparisonValue(string referenceDate) override {
        return 0.0;
    }

public:
    ArrearsMetricStrategy(DashboardRepository& repository)
        : DashboardMetricTemplate(repository) {
    }

    DashboardMetric calculate(string referenceDate) override {
        return build(referenceDate);
    }

    bool supports(DashboardMetricType metricType) override {
        return metricType == DashboardMetricType::ARREARS;
    }
};

// =====================================================
// Concrete Strategy: PendingVerificationMetricStrategy
// =====================================================

class PendingVerificationMetricStrategy : public DashboardMetricTemplate, public IDashboardMetricStrategy {
protected:
    DashboardMetricType getMetricType() override {
        return DashboardMetricType::PENDING_VERIFICATION;
    }

    string getMetricLabel() override {
        return "Pending Verification Records";
    }

    double fetchCurrentValue(string referenceDate) override {
        return dashboardRepository.countResidentDrafts()
            + dashboardRepository.countPaymentDrafts()
            + dashboardRepository.countArrearsSummaryDrafts()
            + dashboardRepository.countQuarterCategoryDrafts()
            + dashboardRepository.countUnitDrafts();
    }

    double fetchComparisonValue(string referenceDate) override {
        return 0.0;
    }

public:
    PendingVerificationMetricStrategy(DashboardRepository& repository)
        : DashboardMetricTemplate(repository) {
    }

    DashboardMetric calculate(string referenceDate) override {
        return build(referenceDate);
    }

    bool supports(DashboardMetricType metricType) override {
        return metricType == DashboardMetricType::PENDING_VERIFICATION;
    }
};

// =====================================================
// Concrete Strategy: QuarterArrearsMetricStrategy
// =====================================================

class QuarterArrearsMetricStrategy : public DashboardMetricTemplate, public IDashboardMetricStrategy {
protected:
    DashboardMetricType getMetricType() override {
        return DashboardMetricType::QUARTER_ARREARS;
    }

    string getMetricLabel() override {
        return "Highest Quarter Arrears";
    }

    double fetchCurrentValue(string referenceDate) override {
        vector<QuarterArrearsAnalysis> rows =
            dashboardRepository.findArrearsGroupedByQuarterCategory();

        double highest = 0.0;

        for (QuarterArrearsAnalysis row : rows) {
            if (row.totalArrearsAmount > highest) {
                highest = row.totalArrearsAmount;
            }
        }

        return highest;
    }

    double fetchComparisonValue(string referenceDate) override {
        return 0.0;
    }

public:
    QuarterArrearsMetricStrategy(DashboardRepository& repository)
        : DashboardMetricTemplate(repository) {
    }

    DashboardMetric calculate(string referenceDate) override {
        return build(referenceDate);
    }

    bool supports(DashboardMetricType metricType) override {
        return metricType == DashboardMetricType::QUARTER_ARREARS;
    }
};

// =====================================================
// Service Layer
// Class: DashboardService
// =====================================================

class DashboardService {
private:
    DashboardRepository& dashboardRepository;
    vector<unique_ptr<IDashboardMetricStrategy>> metricStrategies;

public:
    DashboardService(DashboardRepository& repository)
        : dashboardRepository(repository) {
        metricStrategies.push_back(make_unique<CollectionMetricStrategy>(dashboardRepository));
        metricStrategies.push_back(make_unique<UnitStatusMetricStrategy>(dashboardRepository));
        metricStrategies.push_back(make_unique<ArrearsMetricStrategy>(dashboardRepository));
        metricStrategies.push_back(make_unique<PendingVerificationMetricStrategy>(dashboardRepository));
        metricStrategies.push_back(make_unique<QuarterArrearsMetricStrategy>(dashboardRepository));
    }

    DashboardSummary buildSummary(string referenceDate) {
        CollectionSummary collection =
            calculateCurrentMonthCollection("2026-06-01", "2026-07-01");

        UnitOccupancySummary units =
            calculateUnitOccupancySummary();

        ArrearsOverview arrears =
            calculateUnpaidArrears();

        PendingVerificationSummary pendingVerification =
            calculatePendingVerificationCount();

        vector<QuarterArrearsAnalysis> quarterArrears =
            calculateArrearsByQuarterCategory();

        return {
            collection,
            units,
            arrears,
            pendingVerification,
            quarterArrears
        };
    }

    CollectionSummary calculateCurrentMonthCollection(
        string monthStartDate,
        string nextMonthStartDate
    ) {
        double currentTotal =
            dashboardRepository.sumPaymentsBetween(monthStartDate, nextMonthStartDate);

        double previousTotal =
            dashboardRepository.sumPaymentsBetween("2026-05-01", monthStartDate);

        double differenceAmount = currentTotal - previousTotal;
        double differencePercent = 0.0;

        if (previousTotal != 0) {
            differencePercent = (differenceAmount / previousTotal) * 100.0;
        }

        return {
            currentTotal,
            previousTotal,
            differenceAmount,
            differencePercent
        };
    }

    UnitOccupancySummary calculateUnitOccupancySummary() {
        int occupied = dashboardRepository.countUnitsByStatus("OCCUPIED");
        int vacant = dashboardRepository.countUnitsByStatus("VACANT");

        return {
            occupied,
            vacant,
            occupied + vacant
        };
    }

    ArrearsOverview calculateUnpaidArrears() {
        return {
            dashboardRepository.sumPositiveArrears(),
            dashboardRepository.countResidentsWithArrears()
        };
    }

    PendingVerificationSummary calculatePendingVerificationCount() {
        int residentDrafts = dashboardRepository.countResidentDrafts();
        int paymentDrafts = dashboardRepository.countPaymentDrafts();
        int arrearsDrafts = dashboardRepository.countArrearsSummaryDrafts();
        int quarterCategoryDrafts = dashboardRepository.countQuarterCategoryDrafts();
        int unitDrafts = dashboardRepository.countUnitDrafts();

        int totalDrafts = residentDrafts
            + paymentDrafts
            + arrearsDrafts
            + quarterCategoryDrafts
            + unitDrafts;

        return {
            residentDrafts,
            paymentDrafts,
            arrearsDrafts,
            quarterCategoryDrafts,
            unitDrafts,
            totalDrafts
        };
    }

    vector<QuarterArrearsAnalysis> calculateArrearsByQuarterCategory() {
        return dashboardRepository.findArrearsGroupedByQuarterCategory();
    }

    DashboardMetric calculateMetric(DashboardMetricType metricType, string referenceDate) {
        for (auto& strategy : metricStrategies) {
            if (strategy->supports(metricType)) {
                return strategy->calculate(referenceDate);
            }
        }

        return {
            metricType,
            "Unsupported Metric",
            0.0,
            0.0,
            { 0.0, 0.0, false },
            "Unsupported Metric"
        };
    }
};

// =====================================================
// Structural Pattern: Facade
// Class: DashboardFacade
// =====================================================
// This facade provides simple dashboard loading methods.
// The controller does not need to know how the service calculates each metric.

class DashboardFacade {
private:
    DashboardService& dashboardService;

public:
    DashboardFacade(DashboardService& service)
        : dashboardService(service) {
    }

    DashboardSummary loadDashboardSummary(string referenceDate, string actorId) {
        cout << "Loading dashboard summary for actor: " << actorId << endl;
        return dashboardService.buildSummary(referenceDate);
    }

    CollectionSummary loadCollectionSummary(
        string monthStartDate,
        string previousMonthStartDate
    ) {
        return dashboardService.calculateCurrentMonthCollection(
            monthStartDate,
            previousMonthStartDate
        );
    }

    UnitOccupancySummary loadUnitSummary() {
        return dashboardService.calculateUnitOccupancySummary();
    }

    ArrearsOverview loadArrearsSummary() {
        return dashboardService.calculateUnpaidArrears();
    }

    PendingVerificationSummary loadPendingVerificationSummary() {
        return dashboardService.calculatePendingVerificationCount();
    }

    vector<QuarterArrearsAnalysis> loadArrearsAnalysis() {
        return dashboardService.calculateArrearsByQuarterCategory();
    }
};

// =====================================================
// Controller Layer
// Class: DashboardController
// =====================================================

class DashboardController {
private:
    DashboardFacade& dashboardFacade;
    DashboardMapper dashboardMapper;

public:
    DashboardController(DashboardFacade& facade)
        : dashboardFacade(facade) {
    }

    ApiResponse<DashboardSummary> getDashboardSummary(
        DashboardSummaryRequest request
    ) {
        DashboardSummary summary =
            dashboardFacade.loadDashboardSummary(
                request.referenceDate,
                request.actorId
            );

        DashboardSummary dto = dashboardMapper.toDto(summary);

        return {
            true,
            "Dashboard summary loaded successfully.",
            dto
        };
    }
};

// =====================================================
// Main Function
// =====================================================
// This main function demonstrates how the Dashboard Analytics subsystem works.

int main() {
    DashboardRepository dashboardRepository;
    DashboardService dashboardService(dashboardRepository);
    DashboardFacade dashboardFacade(dashboardService);
    DashboardController dashboardController(dashboardFacade);

    DashboardSummaryRequest request = {
        "2026-06-23",
        "ADMIN_001"
    };

    ApiResponse<DashboardSummary> response =
        dashboardController.getDashboardSummary(request);

    cout << response.message << endl;

    DashboardSummary summary = response.data;

    cout << fixed << setprecision(2);

    cout << "\n===== Collection Summary =====" << endl;
    cout << "Current Month Collection: RM "
         << summary.collection.currentMonthTotal << endl;
    cout << "Previous Month Collection: RM "
         << summary.collection.previousMonthTotal << endl;
    cout << "Difference: RM "
         << summary.collection.differenceAmount << endl;
    cout << "Difference Percent: "
         << summary.collection.differencePercent << "%" << endl;

    cout << "\n===== Unit Occupancy Summary =====" << endl;
    cout << "Occupied Units: " << summary.units.occupiedCount << endl;
    cout << "Vacant Units: " << summary.units.vacantCount << endl;
    cout << "Occupancy Rate: " << summary.units.getOccupancyRate() << "%" << endl;

    cout << "\n===== Arrears Overview =====" << endl;
    cout << "Total Unpaid Arrears: RM "
         << summary.arrears.totalUnpaidArrears << endl;
    cout << "Residents With Arrears: "
         << summary.arrears.residentCount << endl;

    cout << "\n===== Pending Verification Summary =====" << endl;
    cout << "Total Pending Drafts: "
         << summary.pendingVerification.totalDraftCount << endl;

    cout << "\n===== Quarter Arrears Analysis =====" << endl;
    for (QuarterArrearsAnalysis item : summary.quarterArrears) {
        cout << item.categoryName
             << " | Total Arrears: RM "
             << item.totalArrearsAmount
             << " | Average: RM "
             << item.averageArrearsAmount
             << endl;
    }

    QuarterArrearsAnalysis highestRisk =
        summary.getHighestRiskQuarter();

    cout << "\nHighest Risk Quarter Category: "
         << highestRisk.categoryName
         << " with RM "
         << highestRisk.totalArrearsAmount
         << " arrears." << endl;

    DashboardMetric collectionMetric =
        dashboardService.calculateMetric(
            DashboardMetricType::COLLECTION,
            "2026-06-23"
        );

    cout << "\n===== Strategy Metric Example =====" << endl;
    cout << collectionMetric.label << ": RM "
         << collectionMetric.currentValue << endl;
    cout << "Compared Value: RM "
         << collectionMetric.comparisonValue << endl;
    cout << "Delta: "
         << collectionMetric.delta.differencePercent << "%"
         << endl;

    return 0;
}