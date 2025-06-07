import java.util.ArrayList;
import java.util.Scanner;

public class CustomerMenu extends Menu {
    private Customer customer;
    private Order order;
    private ArrayList<Customer> allCustomers = new ArrayList<Customer>();
    private ArrayList<Order> allOrders = new ArrayList<Order>();

    private static final int RECEIPT_WIDTH = 75;
    private static final int ITEM_NAME_WIDTH = 40;
    private static final int SIDE_ORDERS_WIDTH = 40;

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
            System.out.println("Welcome Back! Resuming previous order...");
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
            System.out.println("Please fill in your details before making payment.");
            Customer.readCustomerInfo(read);

            if (!allCustomers.contains(customer)) {
                allCustomers.add(customer);
            }

            FileManager.saveCustomers(allCustomers, "customers.csv");
        }

        System.out.println("\n===== Customer Info =====");
        customer.printCustomerInfo();

        // Print order summary
        System.out.println("\n===== Order Summary =====");
        ArrayList<OrderItem> items = order.getItems();
        if (items.isEmpty()) {
            System.out.println("Your cart is empty. Nothing to pay for.");
            return;
        }

        double total = 0.0;
        for (OrderItem orderItem : items) {
            double subTotal = orderItem.getSubTotal();
            total += subTotal;
            System.out.printf("- %s x%d [%s] = RM%.2f\n",
                    orderItem.getMenuItem().getName(),
                    orderItem.getQuantity(),
                    orderItem.getSideOrders(),
                    subTotal);
        }

        System.out.printf("\nTotal: RM%.2f\n", total);
        
        if (order.getPaymentMethod().equals("Unpaid")) {
            System.out.println("\n===== Payment Method =====");
            String[] paymentMethods = {
                "Cash",
                "Credit Card",
                "Debit Card",
                "E-Wallet (GrabPay)",
                "E-Wallet (Touch'n Go)",
                "DuitNow"
            };
            
            // Display payment options
            for (int i = 0; i < paymentMethods.length; i++) {
                System.out.printf("%d. %s\n", i+1, paymentMethods[i]);
            }
            
            // Get payment selection
            int paymentChoice = 0;
            while (paymentChoice < 1 || paymentChoice > paymentMethods.length) {
                System.out.print("Select payment method (1-" + paymentMethods.length + "): ");
                try {
                    paymentChoice = Integer.parseInt(read.nextLine());
                    if (paymentChoice < 1 || paymentChoice > paymentMethods.length) {
                        System.out.println("Invalid choice! Please enter a number between 1 and " + paymentMethods.length);
                    }
                } catch (NumberFormatException e) {
                    System.out.println("Please enter a valid number!");
                }
            }
            
            this.order.setPaymentMethod(paymentMethods[paymentChoice-1]);
            FileManager.saveOrders(allOrders, "orders.csv");
        }

        printReceipt();

        // Add order to customer's order list, then reset order once user has made payment
        customer.addOrder(order);
        System.out.println("Payment successful. Thank you!");
        order = new Order(customer);
        allOrders.add(order);  // Add the new order to the global list
        FileManager.saveOrders(allOrders, "orders.csv");
    }

    private void printReceipt() {
        System.out.println("\nOrder placed successfully!\n");
        printDivider('=');
        System.out.println(centerText("Receipt"));
        printDivider('=');
        
        // Print order header
        System.out.printf("Order ID      : %d\n", order.getOrderID());
        System.out.printf("Customer Name : %s\n", customer.getName().toUpperCase());
        System.out.printf("Customer Phone: %s\n", customer.getPhone());
        
        String[] dateTime = order.getDateTime().split(" ");
        System.out.printf("Date          : %s\n", dateTime[0]);
        System.out.printf("Time          : %s\n", dateTime[1]);
        System.out.println();
        System.out.printf("Payment Method: %s\n", order.getPaymentMethod());
        System.out.println();
        
        printDivider('-');
        System.out.printf("%-5s %-40s %8s %19s\n", "Code", "Item", "Qty", "Subtotal(RM)");
        printDivider('-');
        
        // Print order items
        ArrayList<OrderItem> items = order.getItems();
        int itemNumber = 1;
        double total = 0;
        
        for (int i = 0; i < items.size(); i++) {
            OrderItem orderItem = items.get(i);
            MenuItem menuItem = orderItem.getMenuItem();
            String itemName = menuItem.getName().toUpperCase();
            String sideOrders = orderItem.getSideOrders();
            double subtotal = orderItem.getSubTotal();
            total += subtotal;
            
            // Wrap item name
            ArrayList<String> nameLines = wrapText(itemName, ITEM_NAME_WIDTH);
            
            // First line with quantity and price
            System.out.printf("%-5d %-40s %8d %19.2f\n", 
                itemNumber, 
                nameLines.get(0), 
                orderItem.getQuantity(), 
                subtotal);
            
            // Additional lines for wrapped item name
            for (int j = 1; j < nameLines.size(); j++) {
                System.out.printf("%-5s %-40s %8s %19s\n", "", nameLines.get(j), "", "");
            }
            
            // Wrap side orders
            String sideLabel = "Side Orders: " + (sideOrders.isEmpty() ? "NO" : sideOrders);
            ArrayList<String> sideLines = wrapText(sideLabel, SIDE_ORDERS_WIDTH);
            
            // Print side orders
            for (int j = 0; j < sideLines.size(); j++) {
                System.out.printf("%-5s %-69s\n", "", sideLines.get(j));
            }
            
            itemNumber++;
        }
        
        printDivider('-');
        System.out.printf("%-55s %19.2f\n", "TOTAL", total);
        printDivider('=');
    }

    private ArrayList<String> wrapText(String text, int width) {
        ArrayList<String> lines = new ArrayList<>();
        if (text == null || text.isEmpty()) {
            lines.add("");
            return lines;
        }
        
        String[] words = text.split(" ");
        StringBuilder currentLine = new StringBuilder();
        
        for (int i = 0; i < words.length; i++) {
            String word = words[i];
            if (currentLine.length() + word.length() > width) {
                lines.add(currentLine.toString());
                currentLine = new StringBuilder();
            }
            if (currentLine.length() > 0) {
                currentLine.append(" ");
            }
            currentLine.append(word);
        }
        
        if (currentLine.length() > 0) {
            lines.add(currentLine.toString());
        }
        
        return lines;
    }

    private String centerText(String text) {
        int spaces = (RECEIPT_WIDTH - text.length()) / 2;
        return String.format("%" + (spaces + text.length()) + "s", text);
    }

    private void printDivider(char c) {
        for (int i = 0; i < RECEIPT_WIDTH; i++) {
            System.out.print(c);
        }
        System.out.println();
    }

    public void addOrderItem(Scanner read) {
        System.out.printf("Enter item name: ");
        String name = read.nextLine().trim();
        
        MenuItem item = findItemByName(name);
        if (item == null) {
            System.out.println("Item not found: " + name);
            return;
        }
        
        int quantity = readValidQuantity(read);
        if (quantity <= 0) return;
        
        String sideOrders = readSideOrders(read);
        addToOrder(item, quantity, sideOrders);
        
        System.out.printf("Added %d x %s to your order\n", quantity, item.getName());
    }

    private int readValidQuantity(Scanner read) {
        while (true) {
            try {
                System.out.printf("Enter quantity: ");
                int quantity = Integer.parseInt(read.nextLine());
                if (quantity > 0) {
                    return quantity;
                }
                System.out.println("Quantity must be greater than 0");
            } catch (NumberFormatException e) {
                System.out.println("Please enter a valid number");
            }
        }
    }

    private String readSideOrders(Scanner read) {
        System.out.printf("Any side orders or instructions? (Press Enter for none): ");
        String input = read.nextLine().trim();
        return input.isEmpty() ? "No additional remarks" : input;
    }

    public void addToOrder(MenuItem item, int quantity, String sideOrders) {
        OrderItem orderItem = new OrderItem(item, quantity, sideOrders);
        order.addItem(orderItem);
        FileManager.saveOrders(allOrders, "orders.csv");
    }

    public void viewOrder() {
        ArrayList<OrderItem> items = order.getItems();
        if (items.isEmpty()) {
            System.out.println("Your cart is empty.");
            return;
        }
        System.out.println("===== Your Cart =====");
        for (OrderItem orderItem : items) {
            System.out.println("- " + orderItem.getMenuItem().getName() + " x" + orderItem.getQuantity() + " [" + orderItem.getSideOrders() + "] = RM" + String.format("%.2f", orderItem.getSubTotal()));
        }
        System.out.printf("Total: RM%.2f\n", order.getTotal());
    }

    public Order getOrder() {
        return order;
    }

    public boolean removeFromOrder(String itemName) {
        ArrayList<OrderItem> items = order.getItems();

        for (int i = 0; i < items.size(); i++) {
            if (items.get(i).getMenuItem().getName().toLowerCase().equals(itemName.toLowerCase())) {
                items.remove(i);
                FileManager.saveOrders(allOrders, "orders.csv");
                return true;
            }
        }
        return false;
    }

    @Override
    public void printMenuOption() {
        super.printMenuOption();
        System.out.println("4. View Existing Order Cart.");
        System.out.println("5. Add Item into Order Cart.");
        System.out.println("6. Remove Item from Order Cart.");
        System.out.println("7. Make payment.");
    }
}
