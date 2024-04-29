
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

function updateLeaderboard() {
    const xhr = new XMLHttpRequest();
    xhr.open("GET", "http://127.0.0.1/leaderboard");
    xhr.send();
    xhr.responseType = "json";
    xhr.onload = () => {
        if (xhr.readyState == 4 && xhr.status == 200) {
            data = xhr.response;
            for (let i = 0; i < data.names.length; i++) {
                // add to leaderboard
            }
        } else {
            console.log("Leaderboard request failed");
        }    
    };
}

function addLine(pos, name, score, time) {
    if (pos % 2 == 0) {
        var background = "lightgrey";
    } else {
        var background = "white";
    }
    document.getElementById("table").innerHTML += `<tr style="background-color: ${background};" ><td>${name}</td><td>${score}</td><td>${time}</td><td>${pos}</td></tr>`;
}