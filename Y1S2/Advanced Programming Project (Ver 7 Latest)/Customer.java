import java.util.ArrayList;
import java.util.Objects;
import java.util.Scanner;

public class Customer {
    private String name;
    private String phone;
    private ArrayList<Order> orders = new ArrayList<Order>();

    public Customer() {
        this("Unknown", "xxx-xxxxxxxx");
    }

    public Customer(String name, String phone) {
        this.name = name;
        this.phone = phone;
    }

    //to Comma-Separated Values String
    public String toCSVString() {
        return String.format("%s,%s", escapeComma(name), escapeComma(phone));
    }

    private static String escapeComma(String value) {
        return value.replace(",", "&#44;"); // Replace comma with &#44
    }

    private static String unescapeComma(String value) {
        return value.replace("&#44;", ",");
    }

    //static because we want to use it before creating instance
    public static Customer fromCSVString(String csvLine) {
        String[] parts = csvLine.split(",", -1);
        if (parts.length < 2) return null;

        return new Customer(unescapeComma(parts[0]), unescapeComma(parts[1]));
    }

    public String getName() {
        return this.name;
    }

    public void setPhone(String phone) {
        this.phone = phone;
    }

    public String getPhone() {
        return this.phone;
    }

    public void addOrder(Order order) {
        this.orders.add(order);
    }

    public ArrayList<Order> getOrders() {
        return this.orders;
    }

    public static Customer readCustomerInfo(Scanner read) {
        String name;
        while (true) {
            System.out.println();
            System.out.printf(ColourManager.ouColour() + "Enter Your Name: " + ColourManager.reColour()); // Output
            System.out.print(ColourManager.inColour()); 
            name = read.nextLine().trim(); // Input
            System.out.print(ColourManager.reColour());
            
            if (name == null || name.isEmpty()) {
                System.out.println(ColourManager.erColour() + "Name cannot be empty! Please try again." + ColourManager.reColour()); // Error
            } else if (name.length() < 2) {
                System.out.println(ColourManager.erColour() + "Name is too short! Please enter at least 2 characters." + ColourManager.reColour()); // Error
            } else if (!name.matches("^[\\p{L} .'-]+$")) {
                System.out.println(ColourManager.erColour() + "Invalid characters in name! Please use only letters, spaces, hyphens, and apostrophes." + ColourManager.reColour()); // Error
            } else {
                break; // Valid name
            }
        }

        String phone;
        while (true) {
            System.out.printf(ColourManager.ouColour() + "Enter Your Phone Number: " + ColourManager.reColour()); // Output
            System.out.print(ColourManager.inColour());
            String input = read.nextLine(); // Input
            System.out.print(ColourManager.reColour());
            
            if (input == null || input.trim().isEmpty()) {
                System.out.println(ColourManager.erColour() + "Phone number cannot be empty!\n" + ColourManager.reColour()); // Error
                continue;
            }
            
            // Normalize phone number (remove all non-digit characters)
            phone = input.replaceAll("[^0-9]", "");
            
            if (phone.length() < 9) {
                System.out.println(ColourManager.erColour() + "Phone number too short! Please enter at least 9 digits.\n" + ColourManager.reColour()); // Error
            } else if (phone.length() > 15) {
                System.out.println(ColourManager.erColour() + "Phone number too long! Maximum 15 digits allowed.\n" + ColourManager.reColour()); // Error
            } else {
                break; // Valid phone number
            }
        }
        
        // Format phone number (e.g., 012-3456789 for Malaysian numbers)
        phone = phone.substring(0, 3) + "-" + phone.substring(3);
        
        return new Customer(name, phone);
    }

    public void printCustomerInfo() {
        System.out.println("Customer Name         : " + this.name);
        System.out.println("Customer Phone Number : " + this.phone);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Customer other = (Customer) obj;
        return this.name.toLowerCase().equals(other.name.toLowerCase()) && this.phone.toLowerCase().equals(other.phone.toLowerCase());
    }

    @Override
    public int hashCode() {
        return Objects.hash(name.toLowerCase(), phone);
    }
}
