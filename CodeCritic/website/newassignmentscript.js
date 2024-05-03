function submit() {
    const xhr = new XMLHttpRequest();
    xhr.open("POST", "http://127.0.0.1/newassignment");
    xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    const body = JSON.stringify({
        name: document.getElementById("assignmentName").value,
        description: document.getElementById("assignmentDescription").value,
        input: document.getElementById("assignmentInput").value,
        output: document.getElementById("assignmentOutput").value,
        exInput: document.getElementById("assignmentExampleInput").value,
        exOutput: document.getElementById("assignmentExampleOutput").value,
        time: document.getElementById("assignmentTime").value,
        constraint: document.getElementById("assignmentConstraint").value,
        tests: document.getElementById("assignmentTestcases").value,
        judge: document.getElementById("assignmentJudge").value,
    });
    xhr.onload = () => {
        if (xhr.readyState == 4 && xhr.status == 201) {
          document.getElementById("assignmentName").value = "";
          document.getElementById("assignmentDescription").value = "";
          document.getElementById("assignmentInput").value = "";
          document.getElementById("assignmentOutput").value = "";
          document.getElementById("assignmentExampleInput").value = "";
          document.getElementById("assignmentExampleOutput").value = "";
          document.getElementById("assigmentTime").value = "";
          document.getElementById("assignmentConstraint").value = "";
          document.getElementById("assignmentTestcases").value = "";
          document.getElementById("assignmentJudge").value = "";
        } else {
          console.log(`Error: ${xhr.status}`);
        }
        checkCookie();
    }
    xhr.send(body);
}