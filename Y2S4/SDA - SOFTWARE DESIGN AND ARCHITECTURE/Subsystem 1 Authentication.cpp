#include <iostream>
#include <string>

using namespace std;

// =====================================================
// Data Transfer Objects and Return Models
// =====================================================

struct ApiResponse {
    bool success;
    string message;
};

struct LoginRequest {
    string email;
    string password;
};

struct LogoutRequest {
    string sessionToken;
};

struct OtpRequest {
    string email;
};

struct RegisterVerifyRequest {
    string email;
    string otp;
    string username;
    string password;
};

struct ForgotVerifyRequest {
    string email;
    string otp;
    string newPassword;
};

struct RegisterVerifyInput {
    string email;
    string otp;
    string username;
    string password;
};

struct ForgotVerifyInput {
    string email;
    string otp;
    string newPassword;
};

struct AuthSession {
    string sessionToken;
    bool isAuthenticated;
};

struct AuthUser {
    string userId;
    string email;
};

struct AdminProfile {
    string adminId;
    string email;
    string username;
};

struct OtpRequestResult {
    bool isSent;
    string message;
};

// =====================================================
// Mock External Supabase Client
// =====================================================
// This class simulates the external Supabase client.
// In the real system, this part represents Supabase SDK/API calls.

class SupabaseClient {
private:
    string clientName;

public:
    SupabaseClient(string name) {
        clientName = name;
    }

    AuthSession signInWithPassword(string email, string password) {
        cout << "[" << clientName << "] Sending login request to Supabase Auth..." << endl;

        if (email.empty() || password.empty()) {
            return { "", false };
        }

        return { "SESSION_TOKEN_123", true };
    }

    bool signOut(string sessionToken) {
        cout << "[" << clientName << "] Sending logout request to Supabase Auth..." << endl;

        return !sessionToken.empty();
    }

    AuthUser getUserBySession(string sessionToken) {
        cout << "[" << clientName << "] Getting current user from Supabase Auth..." << endl;

        if (sessionToken.empty()) {
            return { "", "" };
        }

        return { "ADMIN_001", "admin@johor.gov.my" };
    }

    bool updatePassword(string userId, string newPassword) {
        cout << "[" << clientName << "] Updating password through Supabase Auth..." << endl;

        return !userId.empty() && newPassword.length() >= 8;
    }
};

// =====================================================
// Creational Pattern: Singleton
// Class: SupabaseClientProvider
// =====================================================
// This class ensures that Supabase clients are created once
// and reused by the whole authentication subsystem.

class SupabaseClientProvider {
private:
    SupabaseClient serverClient;
    SupabaseClient adminClient;

    SupabaseClientProvider()
        : serverClient("Server Client"),
          adminClient("Admin Client") {
    }

public:
    SupabaseClientProvider(const SupabaseClientProvider&) = delete;
    SupabaseClientProvider& operator=(const SupabaseClientProvider&) = delete;

    static SupabaseClientProvider& getInstance() {
        static SupabaseClientProvider instance;
        return instance;
    }

    SupabaseClient& getServerClient() {
        return serverClient;
    }

    SupabaseClient& getAdminClient() {
        return adminClient;
    }
};

// =====================================================
// Structural Pattern: Adapter
// Class: SupabaseAuthAdapter
// =====================================================
// This adapter converts the external Supabase authentication
// operations into methods that AuthService can easily use.

class SupabaseAuthAdapter {
private:
    SupabaseClientProvider& clientProvider;

public:
    SupabaseAuthAdapter()
        : clientProvider(SupabaseClientProvider::getInstance()) {
    }

    AuthSession login(string email, string password) {
        return clientProvider
            .getServerClient()
            .signInWithPassword(email, password);
    }

    bool logout(string sessionToken) {
        return clientProvider
            .getServerClient()
            .signOut(sessionToken);
    }

    AuthUser getCurrentUser(string sessionToken) {
        return clientProvider
            .getServerClient()
            .getUserBySession(sessionToken);
    }

    bool changePassword(string userId, string newPassword) {
        return clientProvider
            .getAdminClient()
            .updatePassword(userId, newPassword);
    }
};

// =====================================================
// Service Layer
// Class: AuthService
// =====================================================
// AuthService contains the main authentication business logic.
// It does not directly call SupabaseClient.
// Instead, it uses SupabaseAuthAdapter.

class AuthService {
private:
    SupabaseAuthAdapter supabaseAuthAdapter;

    bool isOfficialJohorEmail(string email) {
        string requiredDomain = "@johor.gov.my";

        if (email.length() < requiredDomain.length()) {
            return false;
        }

        string emailDomain = email.substr(email.length() - requiredDomain.length());
        return emailDomain == requiredDomain;
    }

public:
    AuthSession login(string email, string password) {
        if (!isOfficialJohorEmail(email)) {
            return { "", false };
        }

        return supabaseAuthAdapter.login(email, password);
    }

    bool logout(string sessionToken) {
        return supabaseAuthAdapter.logout(sessionToken);
    }

    OtpRequestResult registerGetOtp(string email) {
        if (!isOfficialJohorEmail(email)) {
            return { false, "Registration OTP failed. Only @johor.gov.my email is allowed." };
        }

        return { true, "Registration OTP has been sent successfully." };
    }

    AdminProfile registerVerify(RegisterVerifyInput input) {
        if (input.email.empty() || input.otp.empty() ||
            input.username.empty() || input.password.length() < 8) {
            return { "", "", "" };
        }

        return { "ADMIN_001", input.email, input.username };
    }

    OtpRequestResult forgotGetOtp(string email) {
        if (!isOfficialJohorEmail(email)) {
            return { false, "Forgot password OTP failed. Only @johor.gov.my email is allowed." };
        }

        return { true, "Forgot password OTP has been sent successfully." };
    }

    bool forgotVerify(ForgotVerifyInput input) {
        if (input.email.empty() || input.otp.empty() || input.newPassword.length() < 8) {
            return false;
        }

        AuthUser currentUser = { "ADMIN_001", input.email };
        return supabaseAuthAdapter.changePassword(currentUser.userId, input.newPassword);
    }

    AdminProfile getCurrentAdmin(string sessionToken) {
        AuthUser currentUser = supabaseAuthAdapter.getCurrentUser(sessionToken);

        if (currentUser.userId.empty()) {
            return { "", "", "" };
        }

        return { currentUser.userId, currentUser.email, "Current Admin" };
    }
};

// =====================================================
// Controller Layer
// Class: AuthController
// =====================================================
// AuthController receives request objects and returns ApiResponse.
// It delegates the authentication process to AuthService.

class AuthController {
private:
    AuthService authService;

public:
    ApiResponse postLogin(LoginRequest request) {
        AuthSession session = authService.login(request.email, request.password);

        if (!session.isAuthenticated) {
            return { false, "Login failed. Invalid email domain or credentials." };
        }

        return { true, "Login successful. Session Token: " + session.sessionToken };
    }

    ApiResponse postLogout(LogoutRequest request) {
        bool isLoggedOut = authService.logout(request.sessionToken);

        if (!isLoggedOut) {
            return { false, "Logout failed. Invalid session token." };
        }

        return { true, "Logout successful." };
    }

    ApiResponse postRegisterGetOtp(OtpRequest request) {
        OtpRequestResult result = authService.registerGetOtp(request.email);

        if (!result.isSent) {
            return { false, result.message };
        }

        return { true, result.message };
    }

    ApiResponse postRegisterVerify(RegisterVerifyRequest request) {
        RegisterVerifyInput input = {
            request.email,
            request.otp,
            request.username,
            request.password
        };

        AdminProfile profile = authService.registerVerify(input);

        if (profile.adminId.empty()) {
            return { false, "Registration verification failed." };
        }

        return { true, "Admin account registered successfully for " + profile.email };
    }

    ApiResponse postForgotGetOtp(OtpRequest request) {
        OtpRequestResult result = authService.forgotGetOtp(request.email);

        if (!result.isSent) {
            return { false, result.message };
        }

        return { true, result.message };
    }

    ApiResponse postForgotVerify(ForgotVerifyRequest request) {
        ForgotVerifyInput input = {
            request.email,
            request.otp,
            request.newPassword
        };

        bool isPasswordChanged = authService.forgotVerify(input);

        if (!isPasswordChanged) {
            return { false, "Forgot password verification failed." };
        }

        return { true, "Password has been reset successfully." };
    }
};

// =====================================================
// Main Function
// =====================================================
// This main function demonstrates how the Authentication subsystem works.

int main() {
    AuthController authController;

    LoginRequest loginRequest = {
        "admin@johor.gov.my",
        "securePass123"
    };

    ApiResponse loginResponse = authController.postLogin(loginRequest);
    cout << loginResponse.message << endl;

    OtpRequest otpRequest = {
        "newadmin@johor.gov.my"
    };

    ApiResponse otpResponse = authController.postRegisterGetOtp(otpRequest);
    cout << otpResponse.message << endl;

    RegisterVerifyRequest registerRequest = {
        "newadmin@johor.gov.my",
        "123456",
        "newAdmin",
        "securePass123"
    };

    ApiResponse registerResponse = authController.postRegisterVerify(registerRequest);
    cout << registerResponse.message << endl;

    LogoutRequest logoutRequest = {
        "SESSION_TOKEN_123"
    };

    ApiResponse logoutResponse = authController.postLogout(logoutRequest);
    cout << logoutResponse.message << endl;

    return 0;
}