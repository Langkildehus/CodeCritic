// Update leaderboard on iframe load
window.addEventListener("load", () => {
    updateLeaderboard();
    updateLanguage();
})

// POST submission to server
function submission() {
    if (document.getElementById("kildekode").value == "") {
        console.log("emoty");
        return;
    }
    modal(1);
    console.log(document.getElementById("kildekode").value);
    // POST request
    const xhr = new XMLHttpRequest();
    xhr.open("POST", "http://127.0.0.1/submit");
    xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xhr.responseType = "json";
    const body = JSON.stringify({
        assignment: "",
        code: document.getElementById("kildekode").value,
    });
    xhr.onload = () => {
        console.log("jeg er lige her");
        if (xhr.readyState == 4 && xhr.status == 201) {
            console.log(xhr.response);
            document.getElementById("notLoggedIn").innerHTML = "";
            resultModal(xhr.response);
            updateLeaderboard();
        } else if (xhr.readyState == 4 && xhr.status == 418) {
            document.getElementById("notLoggedIn").innerHTML = "Please Login to submit";
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
            document.getElementById("table").innerHTML = "<tr><th>Position</th><th>Name</th><th>Points</th><th>Time</th><th>Language</th></tr>";
            for (let i = 0; i < data.length; i++) {
                // Add line to leaderboard using JSON response
                addLine(i+1, data[i].name, data[i].points, data[i].time, data[i].language);
            }
        } else {
            console.log("Leaderboard request failed");
        }    
    };
}

// Add line to leaderboard
function addLine(pos, name, score, time, language) {
    // Style table color
    if (pos % 2 == 0) {
        var background = "lightgrey";
    } else {
        var background = "white";
    }
    // Add line
    document.getElementById("table").innerHTML += `<tr style="background-color: ${background};" ><td>${pos}</td><td>${name}</td><td>${score}</td><td>${time}</td><td>${language}</td></tr>`;
}

// Dropdown
function dropDown() {
    document.getElementById("dropdownContent").classList.toggle("show");
}

// Cookies
function setCookie(cookieName, cookieValue, cookieExp) {
    console.log("hej");
    const date = new Date();
    date.setTime(date.getTime() + cookieExp * 1000 * 60 * 60 * 24);
    let expires = "expires="+date.toUTCString();
    document.cookie = cookieName + "=" + cookieValue + ";" + expires + ";path=/" 
}

function checkCookie(cookieName) {
    let name = cookieName + "=";
    let ca = document.cookie.split(';');
    for (let i = 0; i < ca.length; i++) {
        let c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            return c.substring(name.length, c.length);
        }
    }
    return "";
}

/* Modal open */
function modal(i) {
    document.getElementById(`modal${i}`).style.display = "block";
}

/* Modal close */
function modalClose(i) {
    document.getElementById(`modal${i}`).style.display = "none";
    document.getElementById("results").innerHTML =  '<div class="loader"></div>';
}

function resultModal(data) {
    document.getElementById("results").innerHTML = `<div><p>Points: ${data.points}/${data.maxpoints}</p><p>Time: ${data.time}</p><p>Status: ${data.status}</p></div>`;
}


function updateLanguage() {
    if (checkCookie("language") != "") {
        document.getElementById("language").innerHTML = checkCookie("language") + " ▼";
    } else {
        document.getElementById("language").innerHTML = "Language ▼";
    }
}