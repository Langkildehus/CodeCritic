
function submission() {
    console.log(document.getElementById("kildekode").value);

    const xhr = new XMLHttpRequest();
    xhr.open("POST", "http://127.0.0.1/submit");
    xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    const body = JSON.stringify({
        assignment: "",
        code: document.getElementById("kildekode").value,
    });
    xhr.onload = () => {
        if (xhr.readyState == 4 && xhr.status == 201) {
            console.log("Accepted");
        } else {
          console.log(`Error: ${xhr.status}`);
        }
    }
    xhr.send(body);

    document.getElementById("kildekode").value = "";
}