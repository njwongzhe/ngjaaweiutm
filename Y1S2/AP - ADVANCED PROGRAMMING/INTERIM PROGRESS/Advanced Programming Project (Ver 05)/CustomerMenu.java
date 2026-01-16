import java.util.ArrayList;
import java.util.Scanner;

public class CustomerMenu extends Menu {
    private Customer customer;
    private Order order;
    private ArrayList<Customer> allCustomers = new ArrayList<Customer>();
    private ArrayList<Order> allOrders = new ArrayList<Order>();

    private static final int RECEIPT_WIDTH = 86;
    private static final int ITEM_NAME_WIDTH = 37;
    private static final int REMARKS_WIDTH = 37;

    public CustomerMenu(Customer customer) {
        this.customer = customer;
        this.allCustomers = FileManager.loadCustomers("customers.csv");
        //this.allOrders = FileManager.loadOrders("orders.csv", items); //all customer orders
        // Load all customer orders in allOrders
        for (Customer c : allCustomers) {
            for(Order o : c.getOrders()) {
                allOrders.add(o);
            }
        }

        // Check if this customer already exists in file
        boolean found = false;
        for (Customer c : allCustomers) {
            if (c.equals(customer)) {
                this.customer = c; // Use the stored object with its orders
                found = true;
                break;
            }
        }

        if (!found) {
            allCustomers.add(customer);
            FileManager.saveCustomers(allCustomers, "customers.csv");
        }
        
        // Check for unpaid order to resume
        Order unpaidOrder = null;
        for (Order o : this.customer.getOrders()) {
            if (o.getPaymentMethod().toLowerCase().equals("unpaid")) {
                unpaidOrder = o;
                unpaidOrder.resumeUnpaidOrder();
                break;
            }
        }

        if (unpaidOrder != null) {
            if (!unpaidOrder.getItems().isEmpty()) { 
                System.out.println(ColourManager.suColour() + "Welcome Back! Resuming previous order..." + ColourManager.reColour()); // Success
            } else {
                System.out.println(ColourManager.suColour() + "Welcome Back!" + ColourManager.reColour()); // Success
            }
            this.order = unpaidOrder;
        } else {
            this.order = new Order(customer);
            customer.addOrder(this.order); // Attach new order
            allOrders.add(this.order);
        }

        FileManager.saveOrders(allOrders, "orders.csv");
        FileManager.saveCustomers(allCustomers, "customers.csv");
    }

    public void makePayment(Scanner read) {
        // Ask for name and phone ONLY if still default values
        if (customer.getName().equals("Unknown") || customer.getPhone().equals("xxx-xxxxxxxx")) {
            System.out.println(ColourManager.erColour() + "Please fill in your details before making payment." + ColourManager.reColour()); // Error
            Customer.readCustomerInfo(read);

            if (!allCustomers.contains(customer)) {
                allCustomers.add(customer);
            }

            FileManager.saveCustomers(allCustomers, "customers.csv");
        }

        System.out.println(ColourManager.ouColour() + "\n==================================== Customer Info ===================================" + ColourManager.reColour()); // Output
        customer.printCustomerInfo();

        // Print order summary
        System.out.println(ColourManager.ouColour() + "\n==================================== Order Summary ===================================" + ColourManager.reColour()); // Output
        ArrayList<OrderItem> items = order.getItems();
        if (items.isEmpty()) {
            System.out.println(ColourManager.erColour() + "\nYour cart is empty. Nothing to pay for.\n" + ColourManager.reColour()); // Error
            System.out.println(ColourManager.ouColour() + "======================================================================================" + ColourManager.reColour()); // Output
            return;                              
        }

        viewOrder();
        
        if (order.getPaymentMethod().equals("Unpaid")) {
            System.out.println(ColourManager.ouColour() + "\n=================================== Payment Method ===================================" + ColourManager.reColour()); // Output
            String[] paymentMethods = {
                "Cash",
                "Credit Card",
                "Debit Card",
                "E-Wallet (GrabPay)",
                "E-Wallet (Touch'n Go)",
                "DuitNow"
            };
            
            // Display payment options
            System.out.print(ColourManager.opColour());
            for (int i = 0; i < paymentMethods.length; i++) {
                System.out.printf("%d. %s\n", i+1, paymentMethods[i]);
            }
            System.out.print(ColourManager.reColour());
            
            // Get payment selection
            int paymentChoice = 0;
            while (paymentChoice < 1 || paymentChoice > paymentMethods.length) {
                System.out.print(ColourManager.ouColour() + "Select Payment Method (1 - " + paymentMethods.length + "): " + ColourManager.reColour()); // Output
                try {
                    System.out.print(ColourManager.inColour());
                    paymentChoice = Integer.parseInt(read.nextLine()); // Input
                    System.out.print(ColourManager.reColour());
                    if (paymentChoice < 1 || paymentChoice > paymentMethods.length) {
                        System.out.println(ColourManager.erColour() + "Invalid choice! Please enter a number between 1 and " + paymentMethods.length + "." + ColourManager.reColour() + "\n"); // Error
                    }
                } catch (NumberFormatException e) {
                    System.out.println(ColourManager.erColour() + "Please enter a valid number!" + ColourManager.reColour() + "\n"); // Error
                }
            }
            
            this.order.setPaymentMethod(paymentMethods[paymentChoice-1]);
            FileManager.saveOrders(allOrders, "orders.csv");
        }

        printReceipt();

        // Add order to customer's order list, then reset order once user has made payment
        customer.addOrder(order);
        System.out.println(ColourManager.suColour() + "Payment successful. Thank you!\n" + ColourManager.reColour()); // Success
        System.out.println(ColourManager.ouColour() + "======================================================================================" + ColourManager.reColour()); // Output
        order = new Order(customer);
        allOrders.add(order);  // Add the new order to the global list
        FileManager.saveOrders(allOrders, "orders.csv");
    }

    private void printReceipt() {
        ArrayList<String> receiptLines = new ArrayList<>();
        
        // Generate filename from order date/time and ID
        String filename = "Order_" + order.getDateTime().replace(" ", "_").replace(":", "") + "_ID" + order.getOrderID();
        
        System.out.println(ColourManager.suColour() + "Order placed successfully!\n" + ColourManager.reColour()); // Success
        
        receiptLines.add(generateDivider('='));
        receiptLines.add(centerText("Receipt"));
        receiptLines.add(generateDivider('='));
        
        receiptLines.add(String.format("Order ID       : %d", order.getOrderID()));
        receiptLines.add(String.format("Customer Name  : %s", customer.getName().toUpperCase()));
        receiptLines.add(String.format("Customer Phone : %s", customer.getPhone()));
        
        String[] dateTime = order.getDateTime().split(" ");
        receiptLines.add(String.format("Date           : %s", dateTime[0]));
        receiptLines.add(String.format("Time           : %s", dateTime[1]));
        receiptLines.add("");
        receiptLines.add(String.format("Payment Method : %s", order.getPaymentMethod()));
        
        receiptLines.add(generateDivider('-'));
        receiptLines.add(String.format("%-8s %-37s %8s %10s %19s", "Code", "Item", "Qty", "Price", "Subtotal(RM)"));
        receiptLines.add(generateDivider('-'));
        
        double total = 0;
        for (OrderItem orderItem : order.getItems()) {
            MenuItem menuItem = orderItem.getMenuItem();
            String itemID = menuItem.getMenuItemID();  
            String itemName = menuItem.getName().toUpperCase();
            String remarks = orderItem.getRemarks();
            double price = menuItem.getPrice();
            double subtotal = orderItem.getSubTotal();
            total += subtotal;
            
            receiptLines.add(String.format("%-8s %-37s %8d %10s %19.2f", 
                itemID, 
                wrapText(itemName, ITEM_NAME_WIDTH).get(0), 
                orderItem.getQuantity(), 
                price,
                subtotal));
            
            // Add additional name lines if needed
            ArrayList<String> nameLines = wrapText(itemName, ITEM_NAME_WIDTH);
            for (int j = 1; j < nameLines.size(); j++) {
                receiptLines.add(String.format("%-8s %-37s %8s %10s %19s", "", nameLines.get(j), "", ""));
            }
            
            String remarksLabel = "Remarks: " + (remarks.isEmpty() ? "NO" : remarks);
            ArrayList<String> remarksLines = wrapText(remarksLabel, REMARKS_WIDTH);
            for (String line : remarksLines) {
                receiptLines.add(String.format("%-8s %-77s", "", line));
            }
        }
        
        receiptLines.add(generateDivider('-'));
        receiptLines.add(String.format("%-66s %19.2f", "TOTAL", total));
        receiptLines.add(generateDivider('='));
        
        for (String line : receiptLines) {
            System.out.println(line);
        }
        
        FileManager.saveReceipts(receiptLines, filename);
    }

    // Helper method to generate divider lines
    private String generateDivider(char c) {
        return String.valueOf(c).repeat(RECEIPT_WIDTH);
    }

    // Helper method to center text
    private String centerText(String text) {
        int spaces = (RECEIPT_WIDTH - text.length()) / 2;
        return String.format("%" + (spaces + text.length()) + "s", text);
    }

    private ArrayList<String> wrapText(String text, int width) {
        ArrayList<String> lines = new ArrayList<>();
        if (text == null || text.isEmpty()) {
            return lines;
        }
        
        int currentPosition = 0;
        while (currentPosition < text.length()) {
            // Get substring of max 'width' characters
            int endPosition = Math.min(currentPosition + width, text.length()); // txt.length() is for remaining line (last line)
            String line = text.substring(currentPosition, endPosition);
            lines.add(line);
            currentPosition = endPosition;
        }
        
        return lines;
    }

    public void addOrderItem(Scanner read) {
        System.out.printf(ColourManager.ouColour() + "Enter Item ID or Name: " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String input = read.nextLine().trim(); // Input
        System.out.print(ColourManager.reColour());
        
        MenuItem item = null;
        
        for (MenuItem menuItem : items) {
            if (menuItem.getMenuItemID().toLowerCase().equals(input.toLowerCase())) {
                item = menuItem;
                break;
            }
        }
        
        // If not found by ID, try to find by name
        if (item == null) {
            item = findItemByName(input);
            System.out.println();
            if (item == null) {
                System.out.println(ColourManager.erColour() + "Item not found. Please check the ID or name and try again.\n" + ColourManager.reColour()); // Error
                System.out.println(ColourManager.ouColour() + "===============================================" + ColourManager.reColour()); // Output
                return;
            }
        }
        
        int quantity = readValidQuantity(read);
        if (quantity <= 0) return;
        
        String remarks = readRemarks(read);
        addToOrder(item, quantity, remarks);
        
        System.out.printf(ColourManager.suColour() + "Added %d x %s to your order.\n\n", quantity, item.getName()); // Success
        System.out.println(ColourManager.ouColour() + "===============================================" + ColourManager.reColour()); // Output
        System.out.print(ColourManager.reColour());
    }

    private int readValidQuantity(Scanner read) {
        while (true) {
            try {
                System.out.printf(ColourManager.ouColour() + "Enter Quantity: " + ColourManager.reColour()); // Ask Input
                System.out.print(ColourManager.inColour());
                int quantity = Integer.parseInt(read.nextLine()); // Input
                System.out.print(ColourManager.reColour());
                if (quantity > 0) {
                    return quantity;
                }
                System.out.println(ColourManager.erColour() + "Quantity must be greater than 0!\n" + ColourManager.reColour()); // Error
            } catch (NumberFormatException e) {
                System.out.println(ColourManager.erColour() + "Please enter a valid number!\n" + ColourManager.reColour()); // Error
            }
        }
    }

    private String readRemarks(Scanner read) {
        System.out.printf(ColourManager.ouColour() + "Any Remarks or Instructions? (Press Enter for None): " + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String input = read.nextLine().trim(); // Input
        System.out.print(ColourManager.reColour());
        return input.isEmpty() ? "No additional remarks" : input;
    }

    public void addToOrder(MenuItem item, int quantity, String remarks) {
        OrderItem orderItem = new OrderItem(item, quantity, remarks);
        order.addItem(orderItem);
        FileManager.saveOrders(allOrders, "orders.csv");
    }

    public void viewOrder() {
        ArrayList<OrderItem> items = order.getItems();
        if (items.isEmpty()) {
            System.out.println(ColourManager.erColour() + "Your cart is empty." + ColourManager.reColour()); // Error
            System.out.println(ColourManager.ouColour() + "\n==============================================" + ColourManager.reColour()); // Output
            return;
        }
        
        System.out.println("\n======================================================================================");
        System.out.println(  "                                       Your Cart                                      ");
        System.out.println(  "======================================================================================");
        System.out.printf("%-8s %-37s %8s %10s %19s\n", "ID", "Item", "Qty", "Price", "Subtotal");
        System.out.println("--------------------------------------------------------------------------------------");
        
        for (OrderItem orderItem : items) {
            MenuItem item = orderItem.getMenuItem();
            String remarks = orderItem.getRemarks();
            
            ArrayList<String> nameLines = wrapText(item.getName(), ITEM_NAME_WIDTH);

            // Print main item line
            System.out.printf("%-8s %-37s %8d %10.2f %19.2f\n",
                            item.getMenuItemID(),
                            nameLines.get(0),
                            orderItem.getQuantity(),
                            item.getPrice(),
                            orderItem.getSubTotal());

            for (int i = 1; i < nameLines.size(); i++) {
                System.out.printf("%-8s %-37s %8s %10s %19s\n","", nameLines.get(i), "", "", "");
            }
            
            // Print remarks if they exist
            if (!remarks.isEmpty() && !remarks.toLowerCase().equals("no additional remarks")) {
                ArrayList<String> wrappedRemarks = wrapText("Remarks: " + remarks, 37);
                for (String line : wrappedRemarks) {
                    System.out.printf("%-8s %-25s %5s %10s %15s\n", "", line, "", "", "");
                }
            }
        }
        System.out.println("--------------------------------------------------------------------------------------");
        System.out.printf("%-50s %35.2f\n", "TOTAL:", order.getTotal());
        System.out.println("======================================================================================");
    }

    public Order getOrder() {
        return order;
    }

    public boolean removeFromOrder(Scanner read) {
        System.out.printf(ColourManager.ouColour() + "Enter Item ID or Name to Remove From Cart: "  + ColourManager.reColour()); // Ask Input
        System.out.print(ColourManager.inColour());
        String input = read.nextLine().trim(); // Input
        System.out.print(ColourManager.reColour());
        
        ArrayList<OrderItem> items = order.getItems();
        
        // Try to find by ID first
        for (int i = 0; i < items.size(); i++) {
            OrderItem orderItem = items.get(i);
            if (orderItem.getMenuItem().getMenuItemID().toLowerCase().equals(input.toLowerCase())) {
                items.remove(i);
                FileManager.saveOrders(allOrders, "orders.csv");
                System.out.println(ColourManager.suColour() + "Removed: " + orderItem.getMenuItem().getName() + " (ID: " + orderItem.getMenuItem().getMenuItemID() + ")" + ColourManager.reColour()); // Success
                return true;
            }
        }
        
        // If not found by ID, try by name
        for (int i = 0; i < items.size(); i++) {
            OrderItem orderItem = items.get(i);
            if (orderItem.getMenuItem().getName().toLowerCase().equals(input.toLowerCase())) {
                items.remove(i);
                FileManager.saveOrders(allOrders, "orders.csv");
                System.out.println(ColourManager.suColour() + "Removed: " + orderItem.getMenuItem().getName() + " (ID: " + orderItem.getMenuItem().getMenuItemID() + ")" + ColourManager.reColour()); // Success
                return true;
            }
        }
        
        System.out.println(ColourManager.erColour() + "Item not found in your cart.\n" + ColourManager.reColour()); // Error
        System.out.println(ColourManager.ouColour() + "===============================================" + ColourManager.reColour()); // Output
        return false;
    }

    @Override
    public void printMenuOption() {
        System.out.println();
        System.out.println(ColourManager.ouColour() + "<Customer>" + ColourManager.reColour()); // Output
        super.printMenuOption();
        System.out.print(ColourManager.opColour());
        System.out.println("4. View Existing Order Cart"); // Option
        System.out.println("5. Add Item into Order Cart");
        System.out.println("6. Remove Item from Order Cart");
        System.out.println("7. Make Payment");
        System.out.print(ColourManager.reColour());
    }
}
