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
            System.out.println(ColourManager.ouColour() + "<Admin>" + ColourManager.reColour()); // Output
            System.out.print(ColourManager.opColour());
            System.out.println("0. Exit"); // Option
            System.out.println("1. Admin Login");
            System.out.println("2. Admin Sign Up");
            System.out.print(ColourManager.reColour());

            System.out.printf(ColourManager.ouColour() + "Select an Option (0 - 2): " + ColourManager.reColour()); // Ask Input
            System.out.print(ColourManager.inColour());
            String input = read.nextLine(); // Input
            System.out.print(ColourManager.reColour());

            switch (input) {
                case "0":
                    System.out.println(ColourManager.suColour() + "Exiting Admin access..." + ColourManager.reColour()); // Success
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
                    System.out.println(ColourManager.erColour() + "Invalid option. Try again." + ColourManager.reColour()); // Error
            }
        }
    
        return adminMenu;  // logged in successfully
    }

    private boolean login(Scanner read) {
        System.out.printf(ColourManager.ouColour() + "Enter Admin Name: " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String name = read.nextLine(); // Input
        System.out.print(ColourManager.reColour());

        String password = Admin.readPasswordWithGUI();
        if (password == null) {
            System.out.println(ColourManager.suColour() + "Password input cancelled." + ColourManager.reColour()); // Success
            return false;
        }

        for (Admin admin : admins) {
            if (admin.getName().equals(name) && admin.getPassword().equals(password)) {
                System.out.println(ColourManager.suColour() + "Login successful." + ColourManager.reColour()); // Success
                return true;
            }
        }
        System.out.println(ColourManager.erColour() + "Login failed. Invalid credentials." + ColourManager.reColour()); // Error
        return false;
    }

    private void signUp(Scanner read) {
        Admin newAdmin = Admin.readRegistrationInfo(read, admins);
        if (newAdmin != null) {
            admins.add(newAdmin);
            FileManager.saveAdmins(admins, "admins.csv");  // save updated list back to file
            System.out.println(ColourManager.suColour() + "Registration successful. You can now login." + ColourManager.reColour()); // Success
        } else {
            System.out.println(ColourManager.erColour() + "Registration failed." + ColourManager.reColour()); // Error
        }
    }

    public void addItem(MenuItem item) {
        this.items.add(item);
        saveMenuItems();
        //MenuItem.getAllMenuItems().add(item);  Dont need to add again becasue we have added when readXXXdetails by creating parameterized constructor
    }

    public void removeItem(String input) {
        boolean found = false;
        
        // Try to find by ID first
        for (int i = 0; i < items.size(); i++) {
            MenuItem item = items.get(i);
            if (item.getMenuItemID().toLowerCase().equals(input.toLowerCase())) {
                MenuItem.getAllMenuItems().remove(item);
                MenuItem.recycleMenuItemID(item);
                items.remove(i);
                saveMenuItems();
                System.out.println(ColourManager.suColour() + "Successfully Removed Item: " + item.getName() + " (ID: " + item.getMenuItemID() + ")\n" + ColourManager.reColour()); // Success
                found = true;
                break;
            }
        }
        
        // If not found by ID, try by name
        if (!found) {
            for (int i = 0; i < items.size(); i++) {
                MenuItem item = items.get(i);
                if (item.getName().toLowerCase().equals(input.toLowerCase())) {
                    MenuItem.getAllMenuItems().remove(item);
                    MenuItem.recycleMenuItemID(item);
                    items.remove(i);
                    saveMenuItems();
                    System.out.println(ColourManager.suColour() + "Successfully Removed Item: " + item.getName() + " (ID: " + item.getMenuItemID() + ")\n" + ColourManager.reColour()); // Success
                    found = true;
                    break;
                }
            }
        }
        
        if (!found) {
            System.out.println(ColourManager.erColour() + "\nItem not found. Please check the ID or name and try again.\n" + ColourManager.reColour()); // Error
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
            System.out.println(ColourManager.ouColour() + "What type of item are you adding?" + ColourManager.reColour()); // Output
            System.out.print(ColourManager.opColour());
            System.out.println("0. Cancel"); // Option
            System.out.println("1. Food");
            System.out.println("2. Drink");
            System.out.println("3. Combo");
            System.out.print(ColourManager.reColour());
            System.out.printf(ColourManager.ouColour() + "Select an Option (0 - 3): " + ColourManager.reColour()); // Ask Input
            
            System.out.print(ColourManager.inColour());
            String choice = read.nextLine(); // Input
            System.out.print(ColourManager.reColour());
            MenuItem newItem = null;
            
            switch (choice) {
                case "0":
                    System.out.println(ColourManager.suColour() + "Cancelling item addition." + ColourManager.reColour()); // Success
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
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
                    System.out.println(ColourManager.erColour() + "Invalid choice. Please try again." + ColourManager.reColour()); // Error
                    System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
                    return;
            }

            System.out.printf(ColourManager.ouColour() + "Set item as available? (y/n): " + ColourManager.reColour());
            System.out.print(ColourManager.inColour());
            String availInput = read.nextLine().trim().toLowerCase();
            System.out.print(ColourManager.reColour());
            newItem.setAvailable(availInput.equals("y"));
            
            this.addItem(newItem);
            System.out.println(ColourManager.suColour() + "Item successfully added!" + ColourManager.reColour()); // Success
            System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
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
            System.out.println(ColourManager.erColour() + "\nNo orders found. No popular items to display.\n" + ColourManager.reColour()); // Error
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
        System.out.println(ColourManager.ouColour() + "\n===== Most Popular Items (Ordered " + maxCount + " times) =====\n" + ColourManager.reColour()); // Output
        for (int i = 0; i < popularItems.size(); i++) {
            if (itemCounts.get(i) == maxCount) {
                System.out.println("- " + popularItems.get(i).getName());
            }
        }
    }

    public void filterOrdersByDate(Scanner read) {
        System.out.printf(ColourManager.ouColour() + "Enter Date to Filter (dd-MM-yyyy): " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String dateInput = read.nextLine(); // Input
        System.out.print(ColourManager.reColour());

        // Validate date format
        if (!dateInput.matches("\\d{2}-\\d{2}-\\d{4}")) {
            System.out.println(ColourManager.erColour() + "\nInvalid date format. Please use dd-MM-yyyy format.\n" + ColourManager.reColour()); // Error
            return;
        }
        
        ArrayList<Order> allOrders = FileManager.loadOrders("orders.csv", this.items);
        boolean foundOrders = false;
        
        System.out.println(ColourManager.ouColour() + "\n===== Orders on " + dateInput + " =====\n" + ColourManager.reColour()); // Output
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
                    System.out.printf("- %s x%d [%s] = RM%.2f\n", item.getMenuItem().getName(), item.getQuantity(), item.getRemarks(), item.getSubTotal());
                }
                System.out.printf("Total: RM%.2f\n\n", order.getTotal());
            }
        }
        
        if (!foundOrders) {
            System.out.println(ColourManager.erColour() + "No orders found for this date.\n" + ColourManager.reColour()); // Error
        }
    }

    public void generateDailySalesReport(Scanner read) {
        System.out.println(ColourManager.ouColour() + "\n============================ Daily Report ============================" + ColourManager.reColour()); // Output
        System.out.printf(ColourManager.ouColour() + "\nEnter Date For Sales report (dd-MM-yyyy): " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String dateInput = read.nextLine(); // Input
        System.out.print(ColourManager.reColour());
        
        // Validate date format
        if (!dateInput.matches("\\d{2}-\\d{2}-\\d{4}")) {
            System.out.println(ColourManager.erColour() + "\nInvalid date format. Please use dd-MM-yyyy format.\n" + ColourManager.reColour()); // Error
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
                    MenuItem menuItem = orderItem.getMenuItem();

                    if(orderItem.getMenuItem() instanceof Food) itemType = "FOOD";
                    else if(orderItem.getMenuItem() instanceof Drink) itemType = "DRINK";
                    else if(orderItem.getMenuItem() instanceof Combo) itemType = "COMBO";

                    if (!itemTypes.contains(itemType)) {
                        itemTypes.add(itemType);
                    }
                    salesItems.add(new SalesItem(menuItem.getMenuItemID(), menuItem.getName(), orderItem.getQuantity(), orderItem.getSubTotal(), itemType)); 
                }
            }
        }
        
        if (filteredOrders.isEmpty()) {
            System.out.println(ColourManager.erColour() + "No orders found for this date.\n" + ColourManager.reColour()); // Error
            return;
        }
        
        // Generate the report header
        System.out.println("\n");
        System.out.println("======================================================================");
        System.out.println("                          DAILY SALES REPORT");
        System.out.println("======================================================================");
        
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
                        typeItems.add(new SalesItem(item.itemID , item.name, item.quantity, item.total, item.itemType));
                    }
                    typeTotal += item.total;
                }
            }
            
            grandTotal += typeTotal;
            
            // Print item type header
            System.out.println("\nITEM TYPE: " + itemType);
            System.out.println("----------------------------------------------------------------------");
            System.out.printf("%-8s %-33s %11s %14s\n", "No.", "Item", "Qty Sold", "Total (RM)");
            System.out.println("----------------------------------------------------------------------");
            
            // Print items with text wrapping
            for (int i = 0; i < typeItems.size(); i++) {
                SalesItem item = typeItems.get(i);
                String itemName = item.name;
                int maxLength = 33;
                
                if (itemName.length() > maxLength) {
                    // Split into multiple lines
                    String firstLine = itemName.substring(0, maxLength);
                    String remaining = itemName.substring(maxLength);
                    
                    System.out.printf("%-8s %-33s %11d %14.2f\n", item.itemID, firstLine, item.quantity, item.total);
                    
                    // Print remaining lines without number
                    while (remaining.length() > 0) {
                        String line = remaining.length() > maxLength ? remaining.substring(0, maxLength) : remaining;
                        System.out.printf("%-8s %-33s %11s %14s\n", "", line, "", "");
                        remaining = remaining.length() > maxLength ? remaining.substring(maxLength) : "";
                    }
                } else {
                    System.out.printf("%-8s %-33s %11d %14.2f\n", item.itemID, itemName, item.quantity, item.total);  
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

    public void manageMenuItems(Scanner read) {
        while (true) {
            System.out.println(ColourManager.ouColour() + "\n===== Manage Menu Items =====" + ColourManager.reColour()); // Output
            System.out.print(ColourManager.opColour());
            System.out.println("0. Back to Main Menu"); // Option
            System.out.println("1. Add Menu Item");
            System.out.println("2. Remove Menu Item");
            System.out.println("3. Edit Menu Item");
            System.out.print(ColourManager.reColour());
            System.out.printf(ColourManager.ouColour() + "Select an Option (0 - 3): " + ColourManager.reColour()); // Ask Input

            try {
                System.out.print(ColourManager.inColour());
                int choice = Integer.parseInt(read.nextLine()); // Input
                System.out.print(ColourManager.reColour());

                switch (choice) {
                    case 0:
                        return;
                    case 1:
                        System.out.println(ColourManager.ouColour() + "\n============== Add Item to Menu ==============\n" + ColourManager.reColour()); // Output
                        readAddItem(read);
                        break;
                    case 2:
                        System.out.println(ColourManager.ouColour() + "\n================ Remove Item ================\n" + ColourManager.reColour()); // Output
                        System.out.printf(ColourManager.ouColour() + "Enter Either the Item ID or Item Name to Remove: " + ColourManager.reColour()); // Ask Input
                        System.out.print(ColourManager.inColour());
                        String removeItem = read.nextLine().trim();
                        System.out.print(ColourManager.reColour());
                        removeItem(removeItem);
                        System.out.println(ColourManager.ouColour() + "=============================================" + ColourManager.reColour()); // Output
                        break;
                    case 3:
                        System.out.println(ColourManager.ouColour() + "\n================ Edit Item ================\n" + ColourManager.reColour()); // Output
                        editMenuItem(read);
                        System.out.println(ColourManager.ouColour() + "\n============================================" + ColourManager.reColour()); // Output
                        break;
                    default:
                        System.out.println(ColourManager.erColour() + "Invalid option. Please try again." + ColourManager.reColour()); // Error
                }
            } catch (NumberFormatException e) {
                System.out.println(ColourManager.erColour() + "Please enter a valid number." + ColourManager.reColour()); //Error
            }
        }
    }

    public void editMenuItem(Scanner read) {
        System.out.printf(ColourManager.ouColour() + "Enter Item ID or Name to Edit: " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String input = read.nextLine().trim(); // Input
        System.out.print(ColourManager.reColour());

        MenuItem item = null;
        for (MenuItem menuItem : items) {
            if (menuItem.getMenuItemID().equalsIgnoreCase(input) || 
                menuItem.getName().equalsIgnoreCase(input)) {
                item = menuItem;
                break;
            }
        }

        if (item == null) {
            System.out.println(ColourManager.erColour() + "\nItem not found." + ColourManager.reColour()); // Error
            return;
        }

        System.out.println("\nEditing: " + item.getName() + " (" + item.getMenuItemID() + ")");
        System.out.println("Current Price: RM" + String.format("%.2f", item.getPrice()));
        System.out.println("Current Availability: " + (item.isAvailable() ? ColourManager.suColour() + "Available" : ColourManager.erColour() + "Not Available")); // Success // Error
        System.out.print(ColourManager.reColour());

        while (true) {
            System.out.println(ColourManager.ouColour() + "\n===== Edit Options =====" + ColourManager.reColour()); // Output
            System.out.print(ColourManager.opColour());
            System.out.println("0. Done Editing"); // Option
            System.out.println("1. Edit Price");
            System.out.println("2. Toggle Availability");
            if (item instanceof Combo) {
                System.out.println("3. Edit Exchange Options");
            }
            System.out.print(ColourManager.reColour());
            System.out.printf(ColourManager.ouColour() + "Select an Option (0 - %d): ", (item instanceof Combo) ? 3 : 2); // Output
            System.out.print(ColourManager.reColour());

            try {
                System.out.print(ColourManager.inColour());
                int choice = Integer.parseInt(read.nextLine()); // Input
                System.out.print(ColourManager.reColour());

                switch (choice) {
                    case 0:
                        saveMenuItems();
                        return;
                    case 1:
                        editItemPrice(item, read);
                        break;
                    case 2:
                        item.setAvailable(!item.isAvailable());
                        System.out.println(ColourManager.suColour() + "Availability set to: " + 
                            (item.isAvailable() ? "Available" : "Not Available") + ColourManager.reColour()); // Success
                        break;
                    case 3:
                        if (item instanceof Combo) {
                            editComboExchangeOptions((Combo)item, read);
                        }
                        break;
                    default:
                        System.out.println(ColourManager.erColour() + "Invalid option. Please try again." + ColourManager.reColour()); // Error
                }
            } catch (NumberFormatException e) {
                System.out.println(ColourManager.erColour() + "Please enter a valid number." + ColourManager.reColour()); // Error
            }
        }
    }

    private void editItemPrice(MenuItem item, Scanner read) {
        while (true) {
            try {
                System.out.printf(ColourManager.ouColour() + "\nEnter New Price (Current: RM%.2f): ", item.getPrice()); // Ask Input
                System.out.print(ColourManager.reColour());
                System.out.print(ColourManager.inColour());
                String priceInput = read.nextLine().trim(); // Input
                System.out.print(ColourManager.reColour());

                if (priceInput.isEmpty()) {
                    System.out.println(ColourManager.erColour() + "Price cannot be empty!" + ColourManager.reColour()); // Error
                    continue;
                }

                double newPrice = Double.parseDouble(priceInput);
                if (newPrice < 0) {
                    System.out.println(ColourManager.erColour() + "Price cannot be negative!" + ColourManager.reColour()); // Error
                    continue;
                }

                item.setPrice(newPrice);
                System.out.println(ColourManager.suColour() + "Price updated successfully!" + ColourManager.reColour()); // Success
                return;
            } catch (NumberFormatException e) {
                System.out.println(ColourManager.erColour() + "Invalid price format. Please enter a valid number." + ColourManager.reColour()); // Error
            } catch (Exception e) {
                System.out.println(ColourManager.erColour() + "An error occurred: " + e.getMessage() + ColourManager.reColour()); // Error
            }
        }
    }

    private void editComboExchangeOptions(Combo combo, Scanner read) {
        System.out.println(ColourManager.ouColour() + "\n===== Combo Exchange Options =====" + ColourManager.reColour()); // Output
        
        for (int i = 0; i < combo.getItems().size(); i++) {
            MenuItem comboItem = combo.getItems().get(i);
            boolean exchangeable = combo.getExchangeable().get(i);
            ArrayList<MenuItem> exchangeList = combo.getExchangeList().get(i);
            ArrayList<Double> exchangeFees = combo.getExchangeFees().get(i);

            System.out.println("\nItem: " + comboItem.getName() + " (" + comboItem.getMenuItemID() + ")");
            System.out.println("Currently " + (exchangeable ? ColourManager.suColour() + "Exchangeable" : ColourManager.erColour() + "Not Exchangeable")); // Success // Error
            System.out.print(ColourManager.reColour());
            
            if (exchangeable && !exchangeList.isEmpty()) {
                System.out.println(ColourManager.ouColour() + "Current Exchange Options:" + ColourManager.reColour()); // Output
                System.out.print(ColourManager.opColour()); 
                for (int j = 0; j < exchangeList.size(); j++) { // Option
                    System.out.printf("%d. %s (Extra RM%.2f)\n", 
                        j+1, exchangeList.get(j).getName(), exchangeFees.get(j));
                }
                System.out.print(ColourManager.reColour());
            }

            while (true) {
                try {
                    System.out.printf(ColourManager.ouColour() + "\nMake this item exchangeable? (y/n): " + ColourManager.reColour()); // Ask Input
                    System.out.print(ColourManager.inColour());
                    String choice = read.nextLine().trim().toLowerCase(); // Input
                    System.out.print(ColourManager.reColour());

                    if (!choice.equals("y") && !choice.equals("n")) {
                        System.out.println(ColourManager.erColour() + "Please enter 'y' or 'n'." + ColourManager.reColour()); // Error
                        continue;
                    }

                    boolean newExchangeable = choice.equals("y");
                    combo.getExchangeable().set(i, newExchangeable);

                    if (newExchangeable) {
                        ArrayList<MenuItem> newExchangeList = new ArrayList<>();
                        ArrayList<Double> newExchangeFees = new ArrayList<>();

                        while (true) {
                            try {
                                System.out.println(ColourManager.ouColour() + "\nAvailable Items for Exchange (Same Type):" + ColourManager.reColour()); // Output
                                int counter = 1;
                                ArrayList<MenuItem> sameTypeItems = new ArrayList<>();
                                
                                System.out.print(ColourManager.opColour());
                                for (MenuItem item : items) { // Option
                                    if (!item.equals(comboItem) && 
                                        ((comboItem instanceof Food && item instanceof Food) || 
                                         (comboItem instanceof Drink && item instanceof Drink))) {
                                        System.out.printf("%d. %s (RM%.2f)\n", counter++, item.getName(), item.getPrice());
                                        sameTypeItems.add(item);
                                    }
                                }
                                System.out.print(ColourManager.reColour());

                                if (sameTypeItems.isEmpty()) {
                                    System.out.println(ColourManager.erColour() + "No available items of the same type for exchange." + ColourManager.reColour()); // Error
                                    break;
                                }

                                System.out.printf(ColourManager.ouColour() + "\nSelect An Item to Add to Exchange List (1 - %d OR 0 to Finish): ", sameTypeItems.size()); // Ask Input
                                System.out.print(ColourManager.reColour());
                                System.out.print(ColourManager.inColour());
                                String itemChoiceInput = read.nextLine().trim(); // Input
                                System.out.print(ColourManager.reColour());

                                if (itemChoiceInput.isEmpty()) {
                                    System.out.println(ColourManager.erColour() + "Please enter a valid selection." + ColourManager.reColour()); // Error
                                    continue;
                                }

                                int itemChoice = Integer.parseInt(itemChoiceInput);

                                if (itemChoice == 0) break;
                                if (itemChoice < 1 || itemChoice > sameTypeItems.size()) {
                                    System.out.println(ColourManager.erColour() + "Invalid selection. Please try again." + ColourManager.reColour()); // Error
                                    continue;
                                }

                                MenuItem selectedItem = sameTypeItems.get(itemChoice - 1);
                                while (true) {
                                    try {
                                        System.out.printf(ColourManager.ouColour() + "Enter Extra Fee for Exchanging to %s: RM", selectedItem.getName()); // Ask Input
                                        System.out.print(ColourManager.reColour());
                                        System.out.print(ColourManager.inColour());
                                        String feeInput = read.nextLine().trim(); // Input
                                        System.out.print(ColourManager.reColour());

                                        if (feeInput.isEmpty()) {
                                            System.out.println(ColourManager.erColour() + "Fee cannot be empty!" + ColourManager.reColour()); // Error
                                            continue;
                                        }

                                        double fee = Double.parseDouble(feeInput);
                                        if (fee < 0) {
                                            System.out.println(ColourManager.erColour() + "Fee cannot be negative!" + ColourManager.reColour()); // Error
                                            continue;
                                        }

                                        newExchangeList.add(selectedItem);
                                        newExchangeFees.add(fee);
                                        System.out.println(ColourManager.suColour() + "Exchange option added!" + ColourManager.reColour()); // Success
                                        break;
                                    } catch (NumberFormatException e) {
                                        System.out.println(ColourManager.erColour() + "Invalid fee format. Please enter a valid number." + ColourManager.reColour()); // Error
                                    }
                                }
                            } catch (NumberFormatException e) {
                                System.out.println(ColourManager.erColour() + "Invalid selection. Please enter a number." + ColourManager.reColour()); // Error
                            }
                        }

                        combo.getExchangeList().set(i, newExchangeList);
                        combo.getExchangeFees().set(i, newExchangeFees);
                    }
                    break;
                } catch (Exception e) {
                    System.out.println(ColourManager.erColour() + "An Error Occurred: " + e.getMessage() + ColourManager.reColour()); // Error
                }
            }
        }
        System.out.println(ColourManager.suColour() + "Combo exchange options updated successfully!" + ColourManager.reColour()); // Success
    }

    @Override
    public void printMenuOption() {
        System.out.println();
        System.out.println(ColourManager.ouColour() + "<Admin>" + ColourManager.reColour()); // Output
        super.printMenuOption();
        System.out.print(ColourManager.opColour());
        System.out.println("4. Manage Menu Items"); // Option
        System.out.println("5. View Most Popular Items");
        System.out.println("6. Filter Orders by Date");
        System.out.println("7. Generate Daily Sales Report");
        System.out.println("8. Manage Categories");
        System.out.print(ColourManager.reColour());
    }
}

