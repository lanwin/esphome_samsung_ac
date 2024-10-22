function checkModel() {
    const model = document.getElementById("modelInput").value.toUpperCase();
    const result = document.getElementById("result");

    if (model.length < 11) {
        result.textContent = "Please enter a valid model number.";
        return;
    }

    const classification = model.substring(0, 2);
    const capacity = model.substring(2, 5);
    const productType = model.charAt(6);
    const productNotation = model.charAt(7);
    const feature = model.charAt(8);
    const ratingVoltage = model.charAt(9);
    const mode = model.charAt(10);

    let type = "Other";
    if (productType === "S" || productType === "N" || productType === "X") {
        type = "NASA";
    } else if (productType === "A" || productType === "B" || productType === "C") {
        type = "Non-NASA";
    }

    result.innerHTML = `
        <strong>Model:</strong> ${model} <br>
        <strong>Classification:</strong> ${classification} <br>
        <strong>Capacity:</strong> ${capacity} kW <br>
        <strong>Product Type:</strong> ${productType} (${type}) <br>
        <strong>Product Notation:</strong> ${productNotation} <br>
        <strong>Feature:</strong> ${feature} <br>
        <strong>Rating Voltage:</strong> ${ratingVoltage} <br>
        <strong>Mode:</strong> ${mode}
    `;
}
