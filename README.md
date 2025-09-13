# LKit

**LKit** is a programming language and runtime system designed around three core principles:

---

## 🔑 Core Principles

### 1. Everything is a Handle  
All values, objects, and resources are represented as **handles**.  
- A handle is more than a pointer: it carries **permissions**, **ownership**, and **lifetime** information.  
- You don’t manipulate raw memory—you manipulate handles.  
- This allows seamless tracking of reads, writes, borrows, and destruction without undefined behavior.

---

### 2. Everything is in a Kit (Toolbox)  
All functionality lives inside **kits**, which act as modular toolboxes.  
- No global sprawl: every import, operation, or type comes through a named kit.  
- The standard library is itself just a set of kits (e.g., `std::io::file`).  
- Kits make code **self-contained, explicit, and composable**, with zero hidden dependencies.

---

### 3. Everything is Deterministic  
Every action in LKit has a well-defined result—even if the developer doesn’t spell it out.  
- No hidden randomness, undefined behavior, or “implementation-dependent” quirks.  
- If a developer leaves something unspecified, LKit provides a **deterministic default**.  
- This ensures reproducibility, predictability, and reliable reasoning across all codebases.

---

## ✨ Why LKit?
- Eliminates undefined behavior.  
- Gives fine-grained control over memory and permissions without footguns.  
- Provides a consistent, modular foundation for building systems code or high-level applications alike.  

---

## 📦 Example

```lkit
import std::io::file
import std::io::console

subr factorial (int n) returns int {
  return (n < 2) ? n * factorial(n - 1) : 1;
}

routine main() {
    // create a file handle via kit import
    file::Writer w = file::open("output.txt");
    console.print("yo! this is factorial of 5: %d{factorial(5)}");
    // write deterministically, no surprises
    w.write("Hello, LKit!");
    //when a handle goes out of scope, the resource its bound to is released. 
}
```
Note: this doesn't actually exist yet... this is just the ideal language
