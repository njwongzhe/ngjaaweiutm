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
        System.out.println(ColourManager.ouColour() + "\n================ View Orders =================\n" + ColourManager.reColour()); // Output
        System.out.printf(ColourManager.ouColour() + "Enter Date to View (dd-MM-yyyy): " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String dateInput = read.nextLine(); // Input
        System.out.print(ColourManager.reColour());

        // Validate date format.
        if (!dateInput.matches("\\d{2}-\\d{2}-\\d{4}")) {
            System.out.println(ColourManager.erColour() + "\nInvalid date format. Please use dd-MM-yyyy format." + ColourManager.reColour()); // Error
            System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
            return;
        }
        
        ArrayList<Order> allOrders = FileManager.loadOrders("orders.csv", this.items);
        boolean foundOrders = false;
        
        System.out.println(ColourManager.ouColour() + "\n============ Orders on " + dateInput + " ============" + ColourManager.reColour());
        for (Order order : allOrders) {
            String[] dateTimeParts = order.getDateTime().split(" ");
            String orderDate = dateTimeParts[0]; // dd-MM-yyyy
            String orderTime = dateTimeParts[1]; // HH:mm:ss
            
            if (order.getPaymentMethod().equalsIgnoreCase("unpaid") && order.getItems().isEmpty()) {
                continue;
            }

            if (orderDate.equals(dateInput)) {
                foundOrders = true;
                
                // Print order header.
                System.out.println();
                System.out.println("Order ID: " + order.getOrderID());
                System.out.println("Customer Name: " + order.getCustomer().getName());
                System.out.println("Phone: " + order.getCustomer().getPhone());
                System.out.println("Address: " + order.getAddress());
                System.out.println("Date: " + orderDate);
                System.out.println("Time: " + orderTime.split(":")[0] + ":" + orderTime.split(":")[1]);
                System.out.println("\nOrder Items:");
                
                // Print order items.
                for (OrderItem item : order.getItems()) {
                    System.out.printf("%s. %s - %d x RM %.2f\n",
                        item.getMenuItem().getMenuItemID(),
                        item.getMenuItem().getName(),
                        item.getQuantity(),
                        item.getMenuItem().getPrice());
                }
                
                // Print order footer.
                System.out.printf("Total Amount: RM %.2f\n", order.getTotal());
                System.out.println("Payment Method: " + order.getPaymentMethod());
                System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
            }
        }
        
        if (!foundOrders) {
            System.out.println(ColourManager.erColour() + "\nNo orders found for this date." + ColourManager.reColour()); // Error
            System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
        }
    }

    private void placeOrder(Scanner read) {
        System.out.println(ColourManager.ouColour() + "\n================ Place Order =================" + ColourManager.reColour()); // Output
        Customer customer = Customer.readCustomerInfo(read);
        CustomerMenu customerMenu = new CustomerMenu(customer, read);
        customerMenu.readMenuOption(read);
        System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
    }

    @Override
    public void printMenuOption() {
        System.out.println();
        System.out.println(ColourManager.ouColour() + "<Staff>" + ColourManager.reColour()); // Output
        System.out.print(ColourManager.opColour());
        System.out.println("0. Log Out"); // Option
        System.out.println("1. View Existing Orders");
        System.out.println("2. Place Order");
        System.out.print(ColourManager.reColour());
    }

    @Override
    public boolean readMenuOption(Scanner read) {
        int choice = -1;
        do {
            this.printMenuOption();

            System.out.printf(ColourManager.ouColour() + "Select an Option (0 - 2): " + ColourManager.reColour()); // Ask Input
            try {
                System.out.print(ColourManager.inColour());
                choice = Integer.parseInt(read.nextLine());
                System.out.print(ColourManager.reColour());
                if (choice < 0 || choice > 2) {
                    System.out.println(ColourManager.erColour() + "Only inputs (0 - 2) are accepted. Please try again." + ColourManager.reColour()); // Error
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
                    placeOrder(read);
                    break;
                default:
                    System.out.println(ColourManager.erColour() + "Invalid choice." + ColourManager.reColour()); // Error
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
            }
        } while (choice != 0);
        return true;
    }
}