import java.util.Random;
import java.util.ArrayList;
import java.util.Date;

public class PromoCode {
    private String code;
    private double discountPercentage;
    private boolean isPermanent;
    private Date creationDate;

    public PromoCode(String code, double discountPercentage, boolean isPermanent) {
        if (code == null || code.length() != 6) {
            throw new IllegalArgumentException("Promo code must be 6 characters long");
        }
        if (discountPercentage <= 0 || discountPercentage > 100) {
            throw new IllegalArgumentException("Discount percentage must be between 0 and 100");
        }
        
        this.code = code;
        this.discountPercentage = discountPercentage;
        this.isPermanent = isPermanent;
        this.creationDate = new Date();
    }

    public static String generatePromoCode(ArrayList<PromoCode> existingPromoCodes) {
        String chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        Random random = new Random();
        int maxAttempts = 100; // Prevent infinite loops
        int attempts = 0;
        
        while (attempts < maxAttempts) {
            StringBuilder sb = new StringBuilder(6);
            
            // Generate a random 6-character code
            for (int i = 0; i < 6; i++) {
                sb.append(chars.charAt(random.nextInt(chars.length())));
            }
            
            String generatedCode = sb.toString();
            boolean isDuplicate = false;
            
            // Check against existing codes
            if (existingPromoCodes != null) {
                for (PromoCode promo : existingPromoCodes) {
                    if (promo.getCode().equals(generatedCode)) {
                        isDuplicate = true;
                        break;
                    }
                }
            }
            
            // If not a duplicate, return it
            if (!isDuplicate) {
                return generatedCode;
            }
            
            attempts++;
        }
        
        // If we couldn't find a unique code after maxAttempts
        throw new RuntimeException("Failed to generate a unique promo code after " + maxAttempts + " attempts");
    }

    public String getCode() {
        return code;
    
    }
    public double getDiscountPercentage() { 
        return discountPercentage; 
    }

    public boolean isPermanent() { 
        return isPermanent; 
    }

    public Date getCreationDate() { 
        return creationDate; 
    }

    public void setCreationDate(Date creationDate) {
        this.creationDate = creationDate;
    }
}