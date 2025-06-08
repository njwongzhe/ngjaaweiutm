import java.util.ArrayList;
import java.util.Scanner;

public class AdminMenu extends Menu{
    private ArrayList<Admin> admins = new ArrayList<Admin>();

    public AdminMenu() {
        super();
        // Load admins and customers
        this.admins = FileManager.loadAdmins("admins.csv");
    }

    public static AdminMenu loginOrSignup(Scanner read) {
        AdminMenu adminMenu = new AdminMenu(); // create empty AdminMenu object
    
        boolean loggedIn = false;
        while (!loggedIn) {
            System.out.println();
            System.out.println("<Admin>");
            System.out.println("0. Exit");
            System.out.println("1. Admin Login");
            System.out.println("2. Admin Sign Up");

            System.out.printf("Select an Option (0 - 2): ");
            String input = read.nextLine();

            switch (input) {
                case "0":
                    System.out.println("\nExiting Admin access...\n");
                    return null;  // Return null to indicate no login
                case "1":
                    System.out.println();
                    loggedIn = adminMenu.login(read);
                    break;
                case "2":
                    System.out.println();
                    adminMenu.signUp(read);
                    break;
                default:
                    System.out.println("\nInvalid option. Try again.");
            }
        }
    
        return adminMenu;  // logged in successfully
    }

    private boolean login(Scanner read) {
        System.out.printf("Enter Admin Name: ");
        String name = read.nextLine();

        String password = Admin.readPasswordWithGUI();
        if (password == null) {
            System.out.println("Password input cancelled.");
            return false;
        }

        for (Admin admin : admins) {
            if (admin.getName().equals(name) && admin.getPassword().equals(password)) {
                System.out.println("Login successful.\n");
                return true;
            }
        }
        System.out.println("Login failed. Invalid credentials.");
        return false;
    }

    private void signUp(Scanner read) {
        Admin newAdmin = Admin.readRegistrationInfo(read, admins);
        if (newAdmin != null) {
            admins.add(newAdmin);
            FileManager.saveAdmins(admins, "admins.csv");  // save updated list back to file
            System.out.println("Registration successful. You can now login.");
        } else {
            System.out.println("Registration failed.");
        }
    }

    public void addItem(MenuItem item) {
        this.items.add(item);
        saveMenuItems();
        //MenuItem.getAllMenuItems().add(item);  Dont need to add again becasue we have added when readXXXdetails by creating parameterized constructor
    }

    public void removeItem(String name) {
        boolean found = false;

        for(int i = 0; i < items.size(); i++) {
            if(items.get(i).getName().toLowerCase().equals(name.toLowerCase())) {
                MenuItem itemToDelete = items.get(i);
                MenuItem.getAllMenuItems().remove(itemToDelete);
                MenuItem.recycleMenuItemID(itemToDelete);
                items.remove(i);
                saveMenuItems();
                System.out.println("Deleted.\n");
                found = true;
                break;
            }
        }

        if(!found) {
            System.out.println("Do not find the item you want to delete.\n");
        }
    }

    //FileManager.saveFoods(foods, "foods.csv"); should be saved if instanceof
    private void saveMenuItems() {
        ArrayList<Food> foods = new ArrayList<Food>();
        ArrayList<Drink> drinks = new ArrayList<Drink>();
        ArrayList<Combo> combos = new ArrayList<Combo>();
        for (MenuItem item : items) {
            if (item instanceof Food) foods.add((Food) item);
            else if (item instanceof Drink) drinks.add((Drink) item);
            else if (item instanceof Combo) combos.add((Combo) item);
        }
        FileManager.saveFoods(foods, "foods.csv");
        FileManager.saveDrinks(drinks, "drinks.csv");
        FileManager.saveCombos(combos, "combos.csv");
    }

    public void readAddItem(Scanner read) {
        try {
            System.out.println("\nWhat type of item are you adding?");
            System.out.println("0. Cancel");
            System.out.println("1. Food");
            System.out.println("2. Drink");
            System.out.println("3. Combo");
            System.out.printf("Select an Option (0 - 3): ");
            
            String choice = read.nextLine();
            MenuItem newItem = null;
            
            switch (choice) {
                case "0":
                    System.out.println("Cancelling item addition.\n");
                    return;
                case "1":
                    System.out.println();
                    newItem = Food.readFoodDetails(read);
                    break;
                case "2":
                    System.out.println();
                    newItem = Drink.readDrinkDetails(read);
                    break;
                case "3":
                    System.out.println();
                    newItem = Combo.readComboDetails(read, this.items);
                    break;
                default:
                    System.out.println("Invalid choice. Please try again.\n");
                    return;
            }
            
            this.addItem(newItem);
            System.out.println("\nItem successfully added!\n");
        } catch (RuntimeException e) {
            System.out.println(e.getMessage());
        }
    }

    public void findMostPopularItems() {
        ArrayList<Order> allOrders = FileManager.loadOrders("orders.csv", this.items);
        
        // We'll use two parallel ArrayLists to track items and their counts
        ArrayList<MenuItem> popularItems = new ArrayList<MenuItem>();
        ArrayList<Integer> itemCounts = new ArrayList<Integer>();
        
        // Count occurrences of each menu item in all orders
        for (Order order : allOrders) {
            if (order.getPaymentMethod().toLowerCase().equals("unpaid")) {
                continue;
            }

            for (OrderItem orderItem : order.getItems()) {
                MenuItem item = orderItem.getMenuItem();
                boolean found = false;
                
                // Check if we've seen this item before
                for (int i = 0; i < popularItems.size(); i++) {
                    if (popularItems.get(i).equals(item)) {
                        // Increment the count for this item
                        itemCounts.set(i, itemCounts.get(i) + orderItem.getQuantity());
                        found = true;
                        break;
                    }
                }
                
                // If this is a new item, add it to our lists
                if (!found) {
                    popularItems.add(item);
                    itemCounts.add(orderItem.getQuantity());
                }
            }
        }
        
        if (popularItems.isEmpty()) {
            System.out.println("\nNo orders found. No popular items to display.\n");
            return;
        }
        
        // Find the maximum count
        int maxCount = 0;
        for (int count : itemCounts) {
            if (count > maxCount) {
                maxCount = count;
            }
        }
        
        // Find all items with the max count
        System.out.println("\n===== Most Popular Items (Ordered " + maxCount + " times) =====\n");
        for (int i = 0; i < popularItems.size(); i++) {
            if (itemCounts.get(i) == maxCount) {
                System.out.println("- " + popularItems.get(i).getName());
            }
        }
    }

    public void filterOrdersByDate(Scanner read) {
        System.out.printf("\nEnter Date to Filter (dd-MM-yyyy): ");
        String dateInput = read.nextLine();
        
        // Validate date format
        if (!dateInput.matches("\\d{2}-\\d{2}-\\d{4}")) {
            System.out.println("Invalid date format. Please use dd-MM-yyyy format.\n");
            return;
        }
        
        ArrayList<Order> allOrders = FileManager.loadOrders("orders.csv", this.items);
        boolean foundOrders = false;
        
        System.out.println("\n===== Orders on " + dateInput + " =====\n");
        for (Order order : allOrders) {
            // Skip unpaid orders
            if (order.getPaymentMethod().toLowerCase().equals("unpaid")) {
                continue;
            }

            // Extract date part from the order's datetime (which is in dd-MM-yyyy HH:mm:ss format)
            String orderDate = order.getDateTime().split(" ")[0];
            
            if (orderDate.equals(dateInput)) {
                foundOrders = true;
                System.out.println("Order ID: " + order.getOrderID());
                System.out.println("Customer: " + order.getCustomer().getName());
                System.out.println("Order Time: " + order.getDateTime());
                System.out.println("Payment Method: " + order.getPaymentMethod());
                
                // Print order items
                System.out.println("Items:");
                for (OrderItem item : order.getItems()) {
                    System.out.printf("- %s x%d [%s] = RM%.2f\n", item.getMenuItem().getName(), item.getQuantity(), item.getSideOrders(), item.getSubTotal());
                }
                System.out.printf("Total: RM%.2f\n\n", order.getTotal());
            }
        }
        
        if (!foundOrders) {
            System.out.println("No orders found for this date.\n");
        }
    }

    public void generateDailySalesReport(Scanner read) {
        System.out.printf("\nEnter Date For Sales report (dd-MM-yyyy): ");
        String dateInput = read.nextLine();
        
        // Validate date format
        if (!dateInput.matches("\\d{2}-\\d{2}-\\d{4}")) {
            System.out.println("Invalid date format. Please use dd-MM-yyyy format.\n");
            return;
        }
        
        ArrayList<Order> allOrders = FileManager.loadOrders("orders.csv", this.items);
        ArrayList<Order> filteredOrders = new ArrayList<Order>();
        ArrayList<SalesItem> salesItems = new ArrayList<SalesItem>();
        ArrayList<String> itemTypes = new ArrayList<String>();
        double grandTotal = 0.0;
        
        // Filter orders by date and collect all items
        for (Order order : allOrders) {
            String orderDate = order.getDateTime().split(" ")[0];
            if (orderDate.equals(dateInput) && !order.getPaymentMethod().toLowerCase().equals("unpaid")) {
                filteredOrders.add(order);
                for (OrderItem orderItem : order.getItems()) {
                    String itemType = null;

                    if(orderItem.getMenuItem() instanceof Food) itemType = "FOOD";
                    else if(orderItem.getMenuItem() instanceof Drink) itemType = "DRINK";
                    else if(orderItem.getMenuItem() instanceof Combo) itemType = "COMBO";

                    if (!itemTypes.contains(itemType)) {
                        itemTypes.add(itemType);
                    }
                    salesItems.add(new SalesItem(orderItem.getMenuItem().getName(), orderItem.getQuantity(), orderItem.getSubTotal(), itemType));
                }
            }
        }
        
        if (filteredOrders.isEmpty()) {
            System.out.println("No orders found for this date.\n");
            return;
        }
        
        // Generate the report header
        System.out.println("\n");
        System.out.println("======================================================================");
        System.out.println("                          DAILY SALES REPORT");
        System.out.println("======================================================================");
        System.out.println();
        
        // Process each item type
        for (String itemType : itemTypes) {
            ArrayList<SalesItem> typeItems = new ArrayList<SalesItem>();
            double typeTotal = 0.0;
            
            // Find all items of this type and combine quantities
            for (SalesItem item : salesItems) {
                if (item.itemType.equals(itemType)) {
                    boolean found = false;
                    for (SalesItem existing : typeItems) {
                        if (existing.name.equals(item.name)) {
                            existing.quantity += item.quantity;
                            existing.total += item.total;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        typeItems.add(new SalesItem(item.name, item.quantity, item.total, item.itemType));
                    }
                    typeTotal += item.total;
                }
            }
            
            grandTotal += typeTotal;
            
            // Print item type header
            System.out.println("ITEM TYPE: " + itemType);
            System.out.println("----------------------------------------------------------------------");
            System.out.printf("%-4s %-37s %11s %14s\n", "No.", "Item", "Qty Sold", "Total (RM)");
            System.out.println("----------------------------------------------------------------------");
            
            // Print items with text wrapping
            for (int i = 0; i < typeItems.size(); i++) {
                SalesItem item = typeItems.get(i);
                String itemName = item.name;
                int maxLength = 37;
                
                if (itemName.length() > maxLength) {
                    // Split into multiple lines
                    String firstLine = itemName.substring(0, maxLength);
                    String remaining = itemName.substring(maxLength);
                    
                    System.out.printf("%-4d %-37s %11d %14.2f\n", i + 1, firstLine, item.quantity, item.total);
                    
                    // Print remaining lines without number
                    while (remaining.length() > 0) {
                        String line = remaining.length() > maxLength ? remaining.substring(0, maxLength) : remaining;
                        System.out.printf("%-4s %-37s %11s %14s\n", "", line, "", "");
                        remaining = remaining.length() > maxLength ? remaining.substring(maxLength) : "";
                    }
                } else {
                    System.out.printf("%-4d %-37s %11d %14.2f\n", i + 1, itemName, item.quantity, item.total);
                }
            }
            
            String wrappedItemType = "TOTAL (" + itemType + ")";
            System.out.println("----------------------------------------------------------------------");
            System.out.printf("%-54s %14.2f\n", wrappedItemType, typeTotal);
            System.out.println("----------------------------------------------------------------------");
        }
        
        // Print grand total
        System.out.printf("TOTAL REVENUE: %39s %14.2f\n", "", grandTotal);
        System.out.println("----------------------------------------------------------------------");
        System.out.println("REPORT GENERATED FOR : " + dateInput);
        System.out.println("\n======================================================================\n");
    }

    public void manageCategories(Scanner read) {
        CategoryManager.manageCategories(read);
    }

    @Override
    public void printMenuOption() {
        System.out.println("<Admin>");
        super.printMenuOption();
        System.out.println("4. Add Menu Item");
        System.out.println("5. Remove Menu Item");
        System.out.println("6. View Most Popular Items");
        System.out.println("7. Filter Orders by Date");
        System.out.println("8. Generate Daily Sales Report");
        System.out.println("9. Manage Categories");
    }
}


//load and save orders (Array) so that can analyse the most popular food and filter the date as well as daily summary report