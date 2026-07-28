console.log("🚀 Exotic Command Center Loaded");

const statusButton = document.querySelector(".status");
const nodes = document.querySelectorAll(".graph span, .graph strong");

statusButton.addEventListener("click", () => {
    alert("Exotic Core Online\n\nRegistry ✓\nReflection ✓\nManifest ✓\nGraph ✓\nGenerator ✓");
});

nodes.forEach((node, index) => {
    node.addEventListener("click", () => {
        nodes.forEach(n => n.classList.remove("active"));
        node.classList.add("active");

        const names = [
            "Observer", "Memory", "Prediction",
            "Registry", "Core", "Execution",
            "Reflection", "Manifest", "Generator"
        ];

        statusButton.textContent = names[index] + " Selected";
    });
});