// Submit assignment function (POST request)
function submit() {
    const xhr = new XMLHttpRequest();
    xhr.open("POST", "http://127.0.0.1/newassignment");
    xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    const body = JSON.stringify({
        // Form values to JSON
        name: document.getElementById("assignmentName").value,
        description: document.getElementById("assignmentDescription").value,
        input: document.getElementById("assignmentInput").value,
        output: document.getElementById("assignmentOutput").value,
        exInput: document.getElementById("assignmentExampleInput").value,
        exOutput: document.getElementById("assignmentExampleOutput").value,
        time: document.getElementById("assignmentTime").value,
        points: document.getElementById("assignmentPoints").value,
        constraint: document.getElementById("assignmentConstraint").value,
        tests: document.getElementById("assignmentTestcases").value,
        judge: document.getElementById("assignmentJudge").value,
    });
    xhr.onload = () => {
        if (xhr.readyState == 4 && xhr.status == 201) {
          // If server accept, clear form fields
          document.getElementById("assignmentName").value = "";
          document.getElementById("assignmentDescription").value = "";
          document.getElementById("assignmentInput").value = "";
          document.getElementById("assignmentOutput").value = "";
          document.getElementById("assignmentExampleInput").value = "";
          document.getElementById("assignmentExampleOutput").value = "";
          document.getElementById("assigmentTime").value = "";
          document.getElementById("assignmentPoints").value = "";
          document.getElementById("assignmentConstraint").value = "";
          document.getElementById("assignmentTestcases").value = "";
          document.getElementById("assignmentJudge").value = "";
          document.getElementById("numbererror").innerHTML = "";
          document.getElementById("error").innerHTML = "";
        } else if (xhr.readyState == 4 && xhr.status == 400) {
          document.getElementById("error").innerHTML = "Form parameters not valid";
        } else {
          console.log(`Error: ${xhr.status}`);
        }
    }
    xhr.send(body);
}