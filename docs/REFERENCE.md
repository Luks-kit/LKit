
# 🛠 LKit Language Reference (v0.1)

**LKit** is a high-performance systems programming language focused on **reliability, speed, and safety**.
It achieves C-like performance by enforcing memory, data, and error safety **at compile-time**, eliminating the need for a garbage collector or hidden runtime costs (**Zero-Cost Abstractions**).

---

## 1️⃣ Data Structures & Encapsulation

LKit is **data-centric**, prioritizing **composition** and **explicit access control** over classical object-oriented inheritance.

### Structs (Product Types)
```lkit
struct Name {
    field: Type;
    field2: Type;
};
```

- Aggregates multiple fields into one logical unit.
- **All fields are private by default** → promotes encapsulation.

### Unions (Sum Types / Tagged ADTs)
```lkit
union Result {
    Success { value: T };
    Failure { reason: String };
};
```

- Only one tag is active at a time.
- Perfect for representing **states, errors, or variants**.

### Controlled Access
```lkit
accessible {
    read field;
    modify field2;
};
```

- `read` → read-only access
- `modify` → read/write access

### Methods
```lkit
extend StructName {
    subr print(it: Self read*) { ... }
};
```

- Methods can fully access private fields if using `modify*`.
- Encourages **encapsulation with clear API**.

---

## 2️⃣ Ownership & Memory Safety

Memory safety is enforced **statically** using **exclusive ownership and borrow rules**.
There are **no hidden pointers or unsafe references**.

| Type          | Purpose                               | C Equivalent (Compile-Time) |
|---------------|---------------------------------------|-----------------------------|
| `T*`          | Exclusive Owner → manages memory      | `T*`                       |
| `T read*`     | Immutable Borrow → read-only access  | `const T*`                  |
| `T modify*`   | Mutable Borrow → exclusive read/write | `T*`                        |

### Initialization & Destruction
```lkit
subr init(it: Self modify*, ...) { ... }   // constructor, called via `new`
subr dinit(it: Self*) { ... }              // destructor (RAII, called on scope exit)
```

> **Note:** Every allocated resource must implement `dinit` to avoid leaks.

---

## 3️⃣ Abstraction & Polymorphism (Tools)

LKit **avoids classical inheritance**. Use **tools** (traits/interfaces) to define contracts.

### Tool Definition
```lkit
tool Printable {
    subr print(it: Self read*) : Void;
};
```

- Declares a **set of methods** any implementer must provide.

### Tool Implementation
```lkit
extend StructName with Printable {
    subr print(it: Self read*) { ... }
};
```

### Function Signature
```lkit
subr func(arg: T) : R issues E { ... }
```

- `subr` required
- `:` declares **return type**
- `issues` declares **error type(s)**

> **Tip:** Always declare issues for functions that can fail. This allows the compiler to enforce proper error handling.

---

## 4️⃣ Control Flow & Pattern Matching

The `check` system provides **safe, exhaustive pattern matching**.
The matched expression result is bound to the implicit variable `it`.

| Construct        | Syntax                                      | Behavior |
|------------------|--------------------------------------------|---------|
| Standard Check    | `check (expr) only { (match): {...} ... };` | Executes **first matching clause only** |
| Full Check        | `check (expr) { (match): {...} ... };`      | Executes **all matching clauses sequentially** |
| Iterative Check   | `recheck (expr) { (match): {...} ... };`  | Repeats until no matches or explicit break |

### Pattern Matching Examples
```lkit
(Tag { field: v }): { ... }   // ADT tag match
(it > 10): { ... }            // conditional match
```

> **Tip:** `recheck` is ideal for state machines or iterative cleanup operations.

---

## 5️⃣ Error Handling (Issue Pipe)

Errors are handled via the **isolated Issue Pipe**.
- Non-unwinding → state integrity preserved
- Zero overhead on the happy path

### Triggering Issues
```lkit
subr divide(a: Int, b: Int) issues DivideByZero {
    if b == 0 { issue DivideByZero {}; }
};
```

### Handling Issues
```lkit
attempt {
    divide(10, 0);
} plumb {
    print("Caught divide by zero!");
};
```

### Propagation
```lkit
issue it;              // Re-raise captured issue
let result = ensure risky_func(); // shorthand: attempt { ... } plumb { issue it; }
```

> **Tip:** Always handle issues or propagate explicitly; this ensures **compile-time safety**.

---

## 6️⃣ Performance Guarantees

- **Zero-Cost Abstractions:** Ownership, borrowing, and ADTs compile to optimized machine code.
- **Minimal Runtime Costs:** Only array bounds checks; compiler optimizations remove unnecessary checks.
- **Optimized Loops:** Safe ranges are verified at compile-time → no runtime penalties.

> **Pro Tip:** Favor `T read*` or `T modify*` borrows over copying large structs for **max performance**.

---

## 7️⃣ Quick Reference Table

| Feature | Syntax / Example | Notes |
|---------|-----------------|-------|
| Struct | `struct Name { ... }` | Default private fields |
| Union  | `union Name { Tag1 {...}; }` | Tagged sum types |
| Tool   | `tool T { subr f(...); }` | Interface-like abstraction |
| Function | `subr f(...) : R issues E { ... }` | Explicit return + error types |
| Check   | `check (expr) { ... }` | Exhaustive pattern matching |
| Issue   | `issue it;` | Propagate issue to caller |

---

### ✅ Key Takeaways

- **Memory safety first** → no GC, no hidden runtime costs
- **Explicit ownership & borrowing** → safe & predictable
- **Tools > Inheritance** → structured polymorphism
- **Check & Issue** → exhaustive control flow and error handling
- **Zero-Cost Abstractions** → peak performance without hidden costs
