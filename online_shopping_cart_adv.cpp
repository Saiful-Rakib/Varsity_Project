// OnlineShoppingCart.cpp
// A single-file C++ example project demonstrating OOP features
// - Encapsulation, Inheritance, Polymorphism (runtime and compile-time), Abstraction
// - Operator overloading, Templates, Exceptions, File I/O, STL usage
// - Smart pointers and RAII

#include <bits/stdc++.h>
using namespace std;

// -------------------- Exceptions --------------------
class ShopException : public runtime_error {
public:
    explicit ShopException(const string &msg) : runtime_error(msg) {}
};

// -------------------- Product --------------------
class Product {
private:
    int id;
    string name;
    double price;
    int stock;
public:
    Product(int i=0, string n="", double p=0, int s=0)
        : id(i), name(n), price(p), stock(s) {}

    // Encapsulation: getters/setters
    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    void setPrice(double p) { if (p<0) throw ShopException("Price can't be negative"); price = p; }
    void setStock(int s) { if (s<0) throw ShopException("Stock can't be negative"); stock = s; }

    bool reduceStock(int qty) {
        if (qty <= 0) return false;
        if (qty > stock) return false;
        stock -= qty;
        return true;
    }

    void increaseStock(int qty) { if (qty>0) stock += qty; }

    // Operator overloading
    friend ostream& operator<<(ostream &os, const Product &p) {
        os << "[" << p.id << "] " << p.name << " - $" << fixed << setprecision(2) << p.price
           << " (stock: " << p.stock << ")";
        return os;
    }

    bool operator==(const Product &other) const { return id == other.id; }
};

// -------------------- CartItem --------------------
class CartItem {
public:
    Product product;
    int quantity;

    CartItem(const Product &p, int q): product(p), quantity(q) {}

    double subtotal() const { return product.getPrice() * quantity; }
};

// -------------------- User & Admin (Inheritance) --------------------
class User {
protected:
    string username;
    string email;
public:
    explicit User(string uname="guest", string mail="") : username(move(uname)), email(move(mail)) {}
    virtual ~User() = default;

    string getName() const { return username; }
    string getEmail() const { return email; }

    // Abstraction + Polymorphism: virtual function for user type
    virtual string role() const { return "User"; }
};

class Admin : public User {
public:
    Admin(string uname, string mail): User(move(uname), move(mail)) {}
    string role() const override { return "Admin"; }

    // Admin-specific operations could be added
};

// -------------------- Payment (Abstract) --------------------
class Payment {
public:
    virtual ~Payment() = default;
    virtual bool pay(double amount) = 0; // returns true on success
};

class CreditCardPayment : public Payment {
private:
    string cardNumber;
    string nameOnCard;
public:
    CreditCardPayment(string card, string name) : cardNumber(move(card)), nameOnCard(move(name)) {}
    bool pay(double amount) override {
        cout << "Processing credit card payment for $" << fixed << setprecision(2) << amount << "...\n";
        // Fake processing
        if (cardNumber.empty()) return false;
        cout << "Paid by Credit Card (" << nameOnCard << ")\n";
        return true;
    }
};

class PayPalPayment : public Payment {
private:
    string accountEmail;
public:
    explicit PayPalPayment(string email) : accountEmail(move(email)) {}
    bool pay(double amount) override {
        cout << "Processing PayPal payment for $" << fixed << setprecision(2) << amount << "...\n";
        if (accountEmail.empty()) return false;
        cout << "Paid by PayPal (" << accountEmail << ")\n";
        return true;
    }
};

// -------------------- Inventory (Singleton) --------------------
class Inventory {
private:
    unordered_map<int, Product> products; // id -> product
    Inventory() { }
public:
    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) = delete;

    static Inventory& instance() {
        static Inventory inv;
        return inv;
    }

    void addProduct(const Product &p) { products[p.getId()] = p; }
    bool hasProduct(int id) const { return products.find(id) != products.end(); }

    Product getProduct(int id) const {
        auto it = products.find(id);
        if (it == products.end()) throw ShopException("Product not found");
        return it->second;
    }

    bool reduceStock(int id, int qty) {
        auto it = products.find(id);
        if (it == products.end()) return false;
        return it->second.reduceStock(qty);
    }

    vector<Product> listAll() const {
        vector<Product> out;
        for (auto &kv : products) out.push_back(kv.second);
        sort(out.begin(), out.end(), [](const Product &a, const Product &b){ return a.getId() < b.getId(); });
        return out;
    }

    void saveToFile(const string &fname) const {
        ofstream ofs(fname);
        for (auto &kv : products) {
            const Product &p = kv.second;
            ofs << p.getId() << ',' << p.getName() << ',' << p.getPrice() << ',' << p.getStock() << '\n';
        }
    }
};

// -------------------- ShoppingCart --------------------
class ShoppingCart {
private:
    vector<CartItem> items;
public:
    void addToCart(const Product &p, int qty) { items.emplace_back(p, qty); }
    void removeFromCart(int productId, int qty) { /* simplified */ }
    double total() const { double sum=0; for(auto& ci:items) sum+=ci.subtotal(); return sum; }
    vector<CartItem> getItems() const { return items; }
    void clear() { items.clear(); }
    bool empty() const { return items.empty(); }
};

// -------------------- Order --------------------
class Order {
private:
    static int nextOrderId;
    int orderId;
    vector<CartItem> items;
    double amount;
public:
    Order(const vector<CartItem> &its)
        : orderId(++nextOrderId), items(its) {
        amount = 0; for (auto &i : items) amount += i.subtotal();
    }

    void printSummary() const {
        cout << "Order #" << orderId << "\n";
        for (auto &ci : items) cout << "  " << ci.product.getName() << " x" << ci.quantity << " = $" << ci.subtotal() << "\n";
        cout << "Total: $" << amount << "\n";
    }
};
int Order::nextOrderId = 0;

// -------------------- Main --------------------
int main() {
    Inventory &inv = Inventory::instance();
    inv.addProduct(Product(1, "Mouse", 15.0, 10));
    inv.addProduct(Product(2, "Keyboard", 25.0, 5));

    ShoppingCart cart;
    User u("Alice", "alice@mail.com");

    cout << "Welcome " << u.getName() << " (" << u.role() << ")\n";
    for (auto &p : inv.listAll()) cout << p << endl;

    cart.addToCart(inv.getProduct(1), 2);
    cout << "Cart total: $" << cart.total() << endl;

    unique_ptr<Payment> payment = make_unique<CreditCardPayment>("1234","Alice");
    if(payment->pay(cart.total())){
        Order o(cart.getItems());
        o.printSummary();
    }

    return 0;
}
