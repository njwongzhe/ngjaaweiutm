import java.util.ArrayList;
import java.util.Scanner;

public class StaffMenu extends Menu {
    private ArrayList<Staff> staff = new ArrayList<Staff>();

    public StaffMenu() {
        super();
        // Load staff
        this.staff = FileManager.loadStaff("staff.csv");
    }

    public static StaffMenu loginOrSignup(Scanner read) {
        StaffMenu staffMenu = new StaffMenu();
    
        boolean loggedIn = false;
        while (!loggedIn) {
            System.out.println();
            System.out.println(ColourManager.ouColour() + "<Staff>" + ColourManager.reColour()); // Output
            System.out.print(ColourManager.opColour()); 
            System.out.println("0. Exit"); // Option
            System.out.println("1. Staff Login");
            System.out.println("2. Staff Sign Up");
            System.out.print(ColourManager.reColour());

            System.out.printf(ColourManager.ouColour() + "Select an Option (0 - 2): " + ColourManager.reColour()); // Output
            System.out.print(ColourManager.inColour());
            String input = read.nextLine();
            System.out.print(ColourManager.reColour());

            switch (input) {
                case "0":
                    System.out.println(ColourManager.suColour() + "Exiting Staff access..." + ColourManager.reColour()); // Success
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                    return null;
                case "1":
                    loggedIn = staffMenu.login(read);
                    break;
                case "2":
                    staffMenu.signUp(read);
                    break;
                default:
                    System.out.println(ColourManager.erColour() + "Invalid option. Try again." + ColourManager.reColour()); // Error
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
            }
        }
    
        return staffMenu;
    }

    private boolean login(Scanner read) {
        System.out.println(ColourManager.ouColour() + "\n================ Staff Login =================\n" + ColourManager.reColour()); // Output
        System.out.printf(ColourManager.ouColour() + "Enter Staff Name: " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String name = read.nextLine(); // Input
        System.out.print(ColourManager.reColour());

        String password = Staff.readPasswordWithGUI();
        if (password == null) {
            System.out.println(ColourManager.suColour() + "Password input cancelled." + ColourManager.reColour()); // Success
            System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
            return false;
        }

        for (Staff s : staff) {
            if (s.getName().equals(name) && s.getPassword().equals(password)) {
                System.out.println(ColourManager.suColour() + "Login successful." + ColourManager.reColour()); // Success
                System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                return true;
            }
        }
        System.out.println(ColourManager.erColour() + "\nLogin failed. Invalid credentials." + ColourManager.reColour()); // Error
        System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
        return false;
    }

    private void signUp(Scanner read) {
        Staff newStaff = Staff.readRegistrationInfo(read, staff);
        if (newStaff != null) {
            staff.add(newStaff);
            FileManager.saveStaff(staff, "staff.csv");
            System.out.println(ColourManager.suColour() + "Registration successful. You can now login." + ColourManager.reColour()); // Success
            System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
        } else {
            System.out.println(ColourManager.erColour() + "\nRegistration failed." + ColourManager.reColour()); // Error
            System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
        }
    }

    // Differ from AdminMenu filterOrdersByDate, it will show those unpaid but have existing item in order cart as well.
    private void viewExistingOrders(Scanner read) {
        System.out.println(ColourManager.ouColour() + "\n================ View Orders =================\n" + ColourManager.reColour());
        System.out.printf(ColourManager.ouColour() + "Enter Date to View (dd-MM-yyyy): " + ColourManager.reColour());
        System.out.print(ColourManager.inColour());
        String dateInput = read.nextLine();
        System.out.print(ColourManager.reColour());

        if (!dateInput.matches("\\d{2}-\\d{2}-\\d{4}")) {
            System.out.println(ColourManager.erColour() + "\nInvalid date format. Please use dd-MM-yyyy format." + ColourManager.reColour());
            System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour());
            return;
        }
        
        ArrayList<Order> allOrders = FileManager.loadOrders("orders.csv", this.items);
        ArrayList<Order> filteredOrders = new ArrayList<>();
        
        for (Order order : allOrders) {
            String orderDate = order.getDateTime().split(" ")[0];
            if (orderDate.equals(dateInput)) {
                if (!(order.getPaymentMethod().equalsIgnoreCase("unpaid") && order.getItems().isEmpty())) {
                    filteredOrders.add(order);
                }
            }
        }
        
        System.out.println(ColourManager.ouColour() + "\n============ Orders on " + dateInput + " ============" + ColourManager.reColour()); // POutput
        
        if (filteredOrders.isEmpty()) {
            System.out.println(ColourManager.erColour() + "\nNo orders found for this date." + ColourManager.reColour()); // Error
            System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
        } else {
            printOrders(filteredOrders);
        }
    }

    private void placeOrder(Scanner read) {
        System.out.println(ColourManager.ouColour() + "\n================ Place Order =================" + ColourManager.reColour()); // Output
        Customer customer = Customer.readCustomerInfo(read);
        CustomerMenu customerMenu = new CustomerMenu(customer, read);
        customerMenu.readMenuOption(read);
        System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
    }

    private void filterOrdersByCustomerName(Scanner read) {
        System.out.println(ColourManager.ouColour() + "\n=========== Filter Orders by Customer ==========\n" + ColourManager.reColour()); // Output
        
        // Get date input with validation
        String dateInput = "";
        while (true) {
            System.out.printf(ColourManager.ouColour() + "Enter Date (dd-MM-yyyy): " + ColourManager.reColour()); // Ask Input
            System.out.print(ColourManager.inColour());
            dateInput = read.nextLine().trim(); // Input
            System.out.print(ColourManager.reColour());

            if (dateInput.isEmpty()) {
                System.out.println(ColourManager.erColour() + "Date cannot be empty. Please try again.\n" + ColourManager.reColour()); // Error
                continue;
            }

            if (!dateInput.matches("\\d{2}-\\d{2}-\\d{4}")) {
                System.out.println(ColourManager.erColour() + "Invalid date format. Please use dd-MM-yyyy format.\n" + ColourManager.reColour()); // Error
                continue;
            }
            break;
        }

        // Get customer name keyword with validation
        String nameKeyword = "";
        while (true) {
            System.out.printf(ColourManager.ouColour() + "Enter Customer Name (OR Part of Name): " + ColourManager.reColour()); // Ask Input
            System.out.print(ColourManager.inColour());
            nameKeyword = read.nextLine().trim(); // Input
            System.out.print(ColourManager.reColour());

            if (nameKeyword.isEmpty()) {
                System.out.println(ColourManager.erColour() + "Customer name cannot be empty. Please try again.\n" + ColourManager.reColour()); // Error
                continue;
            }
            break;
        }

        // Load orders with error handling
        ArrayList<Order> allOrders = FileManager.loadOrders("orders.csv", this.items);
        ArrayList<Order> filteredOrders = new ArrayList<>();
        
        for (Order order : allOrders) {
            String orderDate = order.getDateTime().split(" ")[0];
            String customerName = order.getCustomer().getName().toLowerCase();
            
            if (orderDate.equals(dateInput) && 
                customerName.contains(nameKeyword.toLowerCase()) &&
                !(order.getPaymentMethod().toLowerCase().equals("unpaid") && order.getItems().isEmpty())) {
                filteredOrders.add(order);
            }
        }
        
        System.out.println(ColourManager.ouColour() + "\n============ Orders on " + dateInput + " ============\n" + ColourManager.reColour()); // Output
        System.out.println(ColourManager.suColour() + "Searching " + nameKeyword + "..." + ColourManager.reColour()); // Success
        System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
        
        if (filteredOrders.isEmpty()) {
            System.out.println(ColourManager.erColour() + "\nNo orders found matching the criteria." + ColourManager.reColour()); // Error
            System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
        } else {
            printOrders(filteredOrders);
        }
    }

    private void printOrders(ArrayList<Order> orders) {
        orders.sort((o1, o2) -> {
            String time1 = o1.getDateTime().split(" ")[1]; // Get time part
            String time2 = o2.getDateTime().split(" ")[1]; // Get time part
            return time1.compareTo(time2); // Compare times directly (HH:MM:SS format sorts correctly as string)
        });
        
        for (Order order : orders) {
            String[] dateTimeParts = order.getDateTime().split(" ");
            String orderDate = dateTimeParts[0];
            String orderTime = dateTimeParts[1];
            
            System.out.println();
            System.out.println("Order ID: " + ((order.getPaymentMethod().toLowerCase().equals("unpaid")) ? "No order ID yet" : order.getOrderID()));
            System.out.println("Customer Name: " + order.getCustomer().getName());
            System.out.println("Phone: " + order.getCustomer().getPhone());
            System.out.println("Address: " + order.getAddress());
            System.out.println("Date: " + orderDate);
            System.out.println("Time: " + orderTime.split(":")[0] + ":" + orderTime.split(":")[1]);
            System.out.println("\nOrder Items:");
            
            for (OrderItem item : order.getItems()) {
                System.out.printf("%s. %s - %d x RM %.2f\n", item.getMenuItem().getMenuItemID(), item.getMenuItem().getName(), item.getQuantity(), item.getMenuItem().getPrice());
            }
            
            System.out.printf("Total Amount: RM %.2f\n", order.getTotal());
            System.out.println("Payment Method: " + order.getPaymentMethod());
            System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
        }
    }

    private void findPromoCodes(Scanner read) {
        ArrayList<PromoCode> promoCodes = FileManager.loadPromoCodes("promocodes.csv");
        
        System.out.println(ColourManager.ouColour() + "\n============== Find Promo Codes ==============\n" + ColourManager.reColour()); // Output
        System.out.printf(ColourManager.ouColour() + "Enter Search Keyword (Leave Empty to View All): " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String keyword = read.nextLine().trim().toLowerCase(); // Input
        System.out.print(ColourManager.reColour());

        boolean found = false;
        for (PromoCode promo : promoCodes) {
            if (keyword.isEmpty() || promo.getCode().toLowerCase().contains(keyword)) {
                System.out.println("\nCode: " + promo.getCode());
                System.out.println("Discount: " + promo.getDiscountPercentage() + "%");
                System.out.println("Type: " + (promo.isPermanent() ? "Permanent" : "One-Time Use"));
                System.out.println("Created: " + promo.getCreationDate());
                System.out.print(ColourManager.ouColour() + "----------------------------------------------" + ColourManager.reColour()); // Output
                found = true;
            }
        }
        
        if (!found) {
            System.out.print(ColourManager.erColour() + "\nNo promo codes found matching your search." + ColourManager.reColour()); // Error
        }
        System.out.println(ColourManager.ouColour() + "\n\n==============================================" + ColourManager.reColour()); // Output
    }

    private void viewPermanentPromoCodes() {
        ArrayList<PromoCode> promoCodes = FileManager.loadPromoCodes("promocodes.csv");
        ArrayList<PromoCode> permanentPromos = new ArrayList<>();
        
        for (PromoCode promo : promoCodes) {
            if (promo.isPermanent()) {
                permanentPromos.add(promo);
            }
        }
        
        System.out.println(ColourManager.ouColour() + "\n=========== Permanent Promo Codes ============\n" + ColourManager.reColour()); // Output
        
        if (permanentPromos.isEmpty()) {
            System.out.println(ColourManager.erColour() + "No permanent promo codes available." + ColourManager.reColour()); // Error
        } else {
            for (PromoCode promo : permanentPromos) {
                System.out.println("Code: " + promo.getCode());
                System.out.println("Discount: " + promo.getDiscountPercentage() + "%");
                System.out.println("Created: " + promo.getCreationDate());
                System.out.println(ColourManager.ouColour() + "----------------------------------------------" + ColourManager.reColour()); // Output
            }
        }
        System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
    }

    @Override
    public void printMenuOption() {
        System.out.println();
        System.out.println(ColourManager.ouColour() + "<Staff>" + ColourManager.reColour()); // Output
        System.out.print(ColourManager.opColour());
        System.out.println("0. Log Out"); // Option
        System.out.println("1. View Existing Orders");
        System.out.println("2. Filter Orders by Customer");
        System.out.println("3. Place Order");
        System.out.println("4. Find Promo Codes");
        System.out.println("5. View Permanent Promo Codes");
        System.out.print(ColourManager.reColour());
    }

    @Override
    public boolean readMenuOption(Scanner read) {
        int choice = -1;
        do {
            this.printMenuOption();

            System.out.printf(ColourManager.ouColour() + "Select an Option (0 - 5): " + ColourManager.reColour()); // Ask Input
            try {
                System.out.print(ColourManager.inColour());
                choice = Integer.parseInt(read.nextLine());
                System.out.print(ColourManager.reColour());
                if (choice < 0 || choice > 5) {
                    System.out.println(ColourManager.erColour() + "Only inputs (0 - 5) are accepted. Please try again." + ColourManager.reColour()); // Error
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                    continue;
                }
            } catch(NumberFormatException e) {
                System.out.println(ColourManager.erColour() + "Invalid input. Please try again." + ColourManager.reColour()); // Error
                System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                continue;
            }
            
            switch (choice) {
                case 0:
                    System.out.println(ColourManager.suColour() + "\nExiting..." + ColourManager.reColour()); // Success
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                    break;
                case 1:
                    viewExistingOrders(read);
                    break;
                case 2:
                    filterOrdersByCustomerName(read);
                    break;
                case 3:
                    placeOrder(read);
                    break;
                case 4:
                    findPromoCodes(read);
                    break;
                case 5:
                    viewPermanentPromoCodes();
                    break;
                default:
                    System.out.println(ColourManager.erColour() + "Invalid choice." + ColourManager.reColour()); // Error
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
            }
        } while (choice != 0);
        return true;
    }
}