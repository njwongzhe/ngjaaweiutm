#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;

// =====================================================
// Enumerations
// =====================================================

enum class DocumentCategory {
    BAYARAN,
    TUNGGAKAN,
    PENGHUNI,
    KUARTERS
};

enum class ParsingMode {
    PDF,
    EXCEL
};

string categoryToString(DocumentCategory category) {
    switch (category) {
        case DocumentCategory::BAYARAN:
            return "BAYARAN";
        case DocumentCategory::TUNGGAKAN:
            return "TUNGGAKAN";
        case DocumentCategory::PENGHUNI:
            return "PENGHUNI";
        case DocumentCategory::KUARTERS:
            return "KUARTERS";
        default:
            return "UNKNOWN";
    }
}

string parsingModeToString(ParsingMode mode) {
    return mode == ParsingMode::PDF ? "PDF" : "EXCEL";
}

// =====================================================
// DTOs and Interfaces
// =====================================================

struct AdminProfile {
    string id;
    string username;
};

struct UploadedFile {
    string fileName;
    string fileType;
    int fileSize;
    string uploadedBy;
};

struct ExtractRequest {
    UploadedFile file;
    ParsingMode parsingMode;
    AdminProfile actor;
};

template <typename T>
struct ApiResponse {
    bool success;
    string message;
    T data;
};

struct ValidationResult {
    bool ok;
    string message;

    bool isValid() const {
        return ok;
    }

    static ValidationResult fail(string message) {
        return { false, message };
    }

    static ValidationResult success() {
        return { true, "Validation successful." };
    }
};

struct VerificationResult {
    bool success;
    int verifiedCount;
    int skippedCount;
    vector<string> messages;

    bool isSuccess() const {
        return success;
    }

    void addMessage(string message) {
        messages.push_back(message);
    }
};

class IExtractResult {
protected:
    DocumentCategory documentType;
    int recordCount;
    ParsingMode parsingMode;

public:
    IExtractResult(DocumentCategory documentType, int recordCount, ParsingMode parsingMode) {
        this->documentType = documentType;
        this->recordCount = recordCount;
        this->parsingMode = parsingMode;
    }

    virtual int getRecordCount() const {
        return recordCount;
    }

    virtual DocumentCategory getDocumentType() const {
        return documentType;
    }

    virtual ParsingMode getParsingMode() const {
        return parsingMode;
    }

    virtual string getResultName() const = 0;

    virtual void printSummary() const {
        cout << getResultName()
             << " | Category: " << categoryToString(documentType)
             << " | Parsing Mode: " << parsingModeToString(parsingMode)
             << " | Records: " << recordCount
             << endl;
    }

    virtual ~IExtractResult() {}
};

class BayaranExtractResult : public IExtractResult {
private:
    vector<string> paymentRows;

public:
    BayaranExtractResult(vector<string> rows, ParsingMode mode)
        : IExtractResult(DocumentCategory::BAYARAN, static_cast<int>(rows.size()), mode) {
        paymentRows = rows;
    }

    string getResultName() const override {
        return "BayaranExtractResult";
    }
};

class PenghuniExtractResult : public IExtractResult {
private:
    vector<string> residentRows;

public:
    PenghuniExtractResult(vector<string> rows, ParsingMode mode)
        : IExtractResult(DocumentCategory::PENGHUNI, static_cast<int>(rows.size()), mode) {
        residentRows = rows;
    }

    string getResultName() const override {
        return "PenghuniExtractResult";
    }
};

class TunggakanExtractResult : public IExtractResult {
private:
    vector<string> arrearsRows;

public:
    TunggakanExtractResult(vector<string> rows, ParsingMode mode)
        : IExtractResult(DocumentCategory::TUNGGAKAN, static_cast<int>(rows.size()), mode) {
        arrearsRows = rows;
    }

    string getResultName() const override {
        return "TunggakanExtractResult";
    }
};

class KuartersExtractResult : public IExtractResult {
private:
    vector<string> quartersRows;

public:
    KuartersExtractResult(vector<string> rows, ParsingMode mode)
        : IExtractResult(DocumentCategory::KUARTERS, static_cast<int>(rows.size()), mode) {
        quartersRows = rows;
    }

    string getResultName() const override {
        return "KuartersExtractResult";
    }
};

struct ProcessingDraft {
    string id;
    DocumentCategory kind;
    string fileName;
    string fileType;
    int fileSize;
    string uploadedBy;
    string uploadedAt;
    shared_ptr<IExtractResult> extractResult;

    string getSummary() const {
        return "Processing draft " + id + " contains "
            + to_string(extractResult->getRecordCount())
            + " extracted records.";
    }

    bool isKind(DocumentCategory category) const {
        return kind == category;
    }
};

// =====================================================
// Creational Pattern: Singleton
// Class: AIServiceClientProvider
// =====================================================

class AIServiceClient {
private:
    string baseUrl;
    int timeoutMs;

public:
    AIServiceClient(string baseUrl, int timeoutMs) {
        this->baseUrl = baseUrl;
        this->timeoutMs = timeoutMs;
    }

    vector<string> postExtraction(string extractionUrl, UploadedFile file) {
        cout << "Forwarding " << file.fileName
             << " to AI service endpoint: " << extractionUrl << endl;
        cout << "AI service timeout: " << timeoutMs << " ms" << endl;

        return {
            "extracted-row-1",
            "extracted-row-2",
            "extracted-row-3"
        };
    }
};

class AIServiceClientProvider {
private:
    string baseUrl;
    int timeoutMs;
    AIServiceClient client;

    AIServiceClientProvider()
        : baseUrl("https://fastapi-ai-service.example.com"),
          timeoutMs(30000),
          client(baseUrl, timeoutMs) {
    }

public:
    AIServiceClientProvider(const AIServiceClientProvider&) = delete;
    AIServiceClientProvider& operator=(const AIServiceClientProvider&) = delete;

    static AIServiceClientProvider& getInstance() {
        static AIServiceClientProvider instance;
        return instance;
    }

    AIServiceClient& getClient() {
        return client;
    }

    string getBaseUrl() const {
        return baseUrl;
    }
};

// =====================================================
// Structural Pattern: Adapter
// Class: AIExtractionAdapter
// =====================================================

class AIExtractionAdapter {
private:
    AIServiceClientProvider& clientProvider;

public:
    AIExtractionAdapter()
        : clientProvider(AIServiceClientProvider::getInstance()) {
    }

    vector<string> extractRows(
        UploadedFile file,
        DocumentCategory kind,
        ParsingMode parsingMode
    ) {
        string extractionUrl = buildExtractionUrl(kind, parsingMode);
        return clientProvider.getClient().postExtraction(extractionUrl, file);
    }

    bool healthCheck() {
        return !clientProvider.getBaseUrl().empty();
    }

    string buildExtractionUrl(DocumentCategory kind, ParsingMode parsingMode) {
        return clientProvider.getBaseUrl()
            + "/extract/"
            + categoryToString(kind)
            + "?mode="
            + parsingModeToString(parsingMode);
    }
};

// =====================================================
// Behavioral Pattern: Strategy
// Interface: IExtractionStrategy
// =====================================================

class IExtractionStrategy {
public:
    virtual shared_ptr<IExtractResult> extract(
        UploadedFile file,
        ParsingMode parsingMode
    ) = 0;

    virtual bool supports(DocumentCategory kind) = 0;

    virtual ~IExtractionStrategy() {}
};

class PenghuniExtractionStrategy : public IExtractionStrategy {
private:
    AIExtractionAdapter& adapter;

public:
    PenghuniExtractionStrategy(AIExtractionAdapter& adapter)
        : adapter(adapter) {
    }

    shared_ptr<IExtractResult> extract(UploadedFile file, ParsingMode parsingMode) override {
        vector<string> rows = adapter.extractRows(file, DocumentCategory::PENGHUNI, parsingMode);
        return make_shared<PenghuniExtractResult>(rows, parsingMode);
    }

    bool supports(DocumentCategory kind) override {
        return kind == DocumentCategory::PENGHUNI;
    }
};

class BayaranExtractionStrategy : public IExtractionStrategy {
private:
    AIExtractionAdapter& adapter;

public:
    BayaranExtractionStrategy(AIExtractionAdapter& adapter)
        : adapter(adapter) {
    }

    shared_ptr<IExtractResult> extract(UploadedFile file, ParsingMode parsingMode) override {
        vector<string> rows = adapter.extractRows(file, DocumentCategory::BAYARAN, parsingMode);
        return make_shared<BayaranExtractResult>(rows, parsingMode);
    }

    bool supports(DocumentCategory kind) override {
        return kind == DocumentCategory::BAYARAN;
    }
};

class KuartersExtractionStrategy : public IExtractionStrategy {
private:
    AIExtractionAdapter& adapter;

public:
    KuartersExtractionStrategy(AIExtractionAdapter& adapter)
        : adapter(adapter) {
    }

    shared_ptr<IExtractResult> extract(UploadedFile file, ParsingMode parsingMode) override {
        vector<string> rows = adapter.extractRows(file, DocumentCategory::KUARTERS, parsingMode);
        return make_shared<KuartersExtractResult>(rows, parsingMode);
    }

    bool supports(DocumentCategory kind) override {
        return kind == DocumentCategory::KUARTERS;
    }
};

class TunggakanExtractionStrategy : public IExtractionStrategy {
private:
    AIExtractionAdapter& adapter;

public:
    TunggakanExtractionStrategy(AIExtractionAdapter& adapter)
        : adapter(adapter) {
    }

    shared_ptr<IExtractResult> extract(UploadedFile file, ParsingMode parsingMode) override {
        vector<string> rows = adapter.extractRows(file, DocumentCategory::TUNGGAKAN, parsingMode);
        return make_shared<TunggakanExtractResult>(rows, parsingMode);
    }

    bool supports(DocumentCategory kind) override {
        return kind == DocumentCategory::TUNGGAKAN;
    }
};

// =====================================================
// Creational Pattern: Factory Method
// Class: ExtractStrategyFactory
// =====================================================

class ExtractStrategyFactory {
private:
    AIExtractionAdapter& adapter;

public:
    ExtractStrategyFactory(AIExtractionAdapter& adapter)
        : adapter(adapter) {
    }

    unique_ptr<IExtractionStrategy> createStrategy(DocumentCategory kind) {
        if (kind == DocumentCategory::PENGHUNI) {
            return make_unique<PenghuniExtractionStrategy>(adapter);
        }

        if (kind == DocumentCategory::BAYARAN) {
            return make_unique<BayaranExtractionStrategy>(adapter);
        }

        if (kind == DocumentCategory::KUARTERS) {
            return make_unique<KuartersExtractionStrategy>(adapter);
        }

        return make_unique<TunggakanExtractionStrategy>(adapter);
    }
};

// =====================================================
// Behavioral Pattern: Chain of Responsibility
// Validation Context and Handlers
// =====================================================

struct ValidationContext {
    DocumentCategory kind;
    ParsingMode parsingMode;
    AdminProfile actor;
    AIExtractionAdapter* adapter;
};

class ValidationHandler {
private:
    shared_ptr<ValidationHandler> next;

protected:
    virtual ValidationResult validateCurrent(
        UploadedFile file,
        ValidationContext context
    ) = 0;

public:
    shared_ptr<ValidationHandler> setNext(shared_ptr<ValidationHandler> handler) {
        next = handler;
        return handler;
    }

    ValidationResult validate(
        UploadedFile file,
        ValidationContext context
    ) {
        ValidationResult result = validateCurrent(file, context);

        if (!result.isValid()) {
            return result;
        }

        if (next != nullptr) {
            return next->validate(file, context);
        }

        return ValidationResult::success();
    }

    virtual ~ValidationHandler() {}
};

class FileValidationHandler : public ValidationHandler {
protected:
    ValidationResult validateCurrent(
        UploadedFile file,
        ValidationContext context
    ) override {
        if (file.fileName.empty()) {
            return ValidationResult::fail("File name is required.");
        }

        if (file.fileType != "pdf" && file.fileType != "xlsx") {
            return ValidationResult::fail("Only PDF and XLSX files are supported.");
        }

        if (file.fileSize <= 0) {
            return ValidationResult::fail("File size must be greater than zero.");
        }

        return ValidationResult::success();
    }
};

class BusinessRuleValidationHandler : public ValidationHandler {
protected:
    ValidationResult validateCurrent(
        UploadedFile file,
        ValidationContext context
    ) override {
        if (context.kind == DocumentCategory::BAYARAN && file.fileType != "xlsx") {
            return ValidationResult::fail("Payment extraction requires an XLSX payment listing.");
        }

        if (context.kind == DocumentCategory::TUNGGAKAN && file.fileType != "xlsx") {
            return ValidationResult::fail("Arrears extraction requires an XLSX arrears listing.");
        }

        return ValidationResult::success();
    }
};

class PermissionValidationHandler : public ValidationHandler {
protected:
    ValidationResult validateCurrent(
        UploadedFile file,
        ValidationContext context
    ) override {
        if (context.actor.id.empty()) {
            return ValidationResult::fail("Extraction denied because the actor is not authenticated.");
        }

        return ValidationResult::success();
    }
};

class ExtractResultValidationHandler : public ValidationHandler {
protected:
    ValidationResult validateCurrent(
        UploadedFile file,
        ValidationContext context
    ) override {
        if (context.adapter == nullptr || !context.adapter->healthCheck()) {
            return ValidationResult::fail("AI extraction service is currently unavailable.");
        }

        return ValidationResult::success();
    }
};

// =====================================================
// Service Layer
// Class: ExtractionGateway
// =====================================================

class ExtractionGateway {
private:
    AIExtractionAdapter adapter;
    ExtractStrategyFactory strategyFactory;

    shared_ptr<ValidationHandler> buildValidationChain() {
        shared_ptr<ValidationHandler> fileHandler = make_shared<FileValidationHandler>();
        shared_ptr<ValidationHandler> businessHandler = make_shared<BusinessRuleValidationHandler>();
        shared_ptr<ValidationHandler> permissionHandler = make_shared<PermissionValidationHandler>();
        shared_ptr<ValidationHandler> resultHandler = make_shared<ExtractResultValidationHandler>();

        fileHandler->setNext(businessHandler)
            ->setNext(permissionHandler)
            ->setNext(resultHandler);

        return fileHandler;
    }

public:
    ExtractionGateway()
        : adapter(),
          strategyFactory(adapter) {
    }

    shared_ptr<IExtractResult> handleExtractRequest(
        UploadedFile file,
        DocumentCategory kind,
        ParsingMode parsingMode,
        AdminProfile actor
    ) {
        ValidationContext context = {
            kind,
            parsingMode,
            actor,
            &adapter
        };

        ValidationResult validation =
            buildValidationChain()->validate(file, context);

        if (!validation.isValid()) {
            cout << "Extraction rejected: " << validation.message << endl;
            return nullptr;
        }

        return forwardToAIService(file, kind, parsingMode);
    }

    ValidationResult validateFileType(string fileName, DocumentCategory kind) {
        if (fileName.find(".pdf") == string::npos &&
            fileName.find(".xlsx") == string::npos) {
            return ValidationResult::fail("Invalid file type.");
        }

        return ValidationResult::success();
    }

    ValidationResult validateFileSize(int fileSize) {
        if (fileSize <= 0) {
            return ValidationResult::fail("Invalid file size.");
        }

        return ValidationResult::success();
    }

    shared_ptr<IExtractResult> forwardToAIService(
        UploadedFile file,
        DocumentCategory kind,
        ParsingMode parsingMode
    ) {
        unique_ptr<IExtractionStrategy> strategy =
            strategyFactory.createStrategy(kind);

        if (!strategy->supports(kind)) {
            return nullptr;
        }

        return strategy->extract(file, parsingMode);
    }
};

// =====================================================
// Controller Layer
// Class: ExtractController
// =====================================================

class ExtractController {
private:
    ExtractionGateway extractionGateway;

    ApiResponse<shared_ptr<IExtractResult>> buildResponse(
        ExtractRequest request,
        DocumentCategory kind
    ) {
        shared_ptr<IExtractResult> result =
            extractionGateway.handleExtractRequest(
                request.file,
                kind,
                request.parsingMode,
                request.actor
            );

        if (result == nullptr) {
            return {
                false,
                "Extraction failed.",
                nullptr
            };
        }

        return {
            true,
            "Extraction completed successfully.",
            result
        };
    }

public:
    ApiResponse<shared_ptr<IExtractResult>> postExtractPenghuni(
        ExtractRequest request
    ) {
        return buildResponse(request, DocumentCategory::PENGHUNI);
    }

    ApiResponse<shared_ptr<IExtractResult>> postExtractBayaran(
        ExtractRequest request
    ) {
        return buildResponse(request, DocumentCategory::BAYARAN);
    }

    ApiResponse<shared_ptr<IExtractResult>> postExtractKuarters(
        ExtractRequest request
    ) {
        return buildResponse(request, DocumentCategory::KUARTERS);
    }

    ApiResponse<shared_ptr<IExtractResult>> postExtractTunggakan(
        ExtractRequest request
    ) {
        return buildResponse(request, DocumentCategory::TUNGGAKAN);
    }
};

// =====================================================
// Main Function
// =====================================================

int main() {
    ExtractController extractController;

    AdminProfile admin = {
        "ADMIN_001",
        "aiAdmin"
    };

    ExtractRequest penghuniRequest = {
        {
            "senarai_penghuni_jun_2026.xlsx",
            "xlsx",
            2048,
            admin.id
        },
        ParsingMode::EXCEL,
        admin
    };

    ApiResponse<shared_ptr<IExtractResult>> penghuniResponse =
        extractController.postExtractPenghuni(penghuniRequest);

    cout << "\n===== Penghuni Extraction =====" << endl;
    cout << penghuniResponse.message << endl;

    if (penghuniResponse.success) {
        penghuniResponse.data->printSummary();
    }

    ExtractRequest bayaranRequest = {
        {
            "bayaran_jun_2026.xlsx",
            "xlsx",
            4096,
            admin.id
        },
        ParsingMode::EXCEL,
        admin
    };

    ApiResponse<shared_ptr<IExtractResult>> bayaranResponse =
        extractController.postExtractBayaran(bayaranRequest);

    cout << "\n===== Bayaran Extraction =====" << endl;
    cout << bayaranResponse.message << endl;

    if (bayaranResponse.success) {
        bayaranResponse.data->printSummary();
    }

    ExtractRequest kuartersRequest = {
        {
            "kuarters_master_list.pdf",
            "pdf",
            3072,
            admin.id
        },
        ParsingMode::PDF,
        admin
    };

    ApiResponse<shared_ptr<IExtractResult>> kuartersResponse =
        extractController.postExtractKuarters(kuartersRequest);

    cout << "\n===== Kuarters Extraction =====" << endl;
    cout << kuartersResponse.message << endl;

    if (kuartersResponse.success) {
        kuartersResponse.data->printSummary();
    }

    ExtractRequest tunggakanRequest = {
        {
            "tunggakan_jun_2026.xlsx",
            "xlsx",
            5120,
            admin.id
        },
        ParsingMode::EXCEL,
        admin
    };

    ApiResponse<shared_ptr<IExtractResult>> tunggakanResponse =
        extractController.postExtractTunggakan(tunggakanRequest);

    cout << "\n===== Tunggakan Extraction =====" << endl;
    cout << tunggakanResponse.message << endl;

    if (tunggakanResponse.success) {
        tunggakanResponse.data->printSummary();
    }

    return 0;
}
