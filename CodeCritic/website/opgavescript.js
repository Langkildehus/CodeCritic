// Update leaderboard on iframe load
window.addEventListener("load", () => {
    updateLeaderboard()
})

// POST submission to server
function submission() {
    console.log(document.getElementById("kildekode").value);
    // POST request
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

    // Clear sumbission textarea
    document.getElementById("kildekode").value = "";
}

// Update leaderboard
function updateLeaderboard() {
    // GET request leaderboard
    const xhr = new XMLHttpRequest();
    xhr.open("GET", "http://127.0.0.1/leaderboard");
    xhr.send();
    xhr.responseType = "json";
    xhr.onload = () => {
        if (xhr.readyState == 4 && xhr.status == 200) {
            data = xhr.response;
            for (let i = 0; i < data.length; i++) {
                // Add line to leaderboard using JSON response
                addLine(i+1, data[i].name, data[i].points, data[i].time);
            }
        } else {
            console.log("Leaderboard request failed");
        }    
    };
}

// Add line to leaderboard
function addLine(pos, name, score, time) {
    // Style table color
    if (pos % 2 == 0) {
        var background = "lightgrey";
    } else {
        var background = "white";
    }
    // Add line
    document.getElementById("table").innerHTML += `<tr style="background-color: ${background};" ><td>${pos}</td><td>${name}</td><td>${score}</td><td>${time}</td></tr>`;
}