// Website load functionality
window.addEventListener("load", () => {
    /* GET request - Opgaver */
    const xhr = new XMLHttpRequest();
    xhr.open("GET", "/opgaver");
    xhr.send();
    xhr.responseType = "json";
    xhr.onload = () => {
    if (xhr.readyState == 4 && xhr.status == 200) {
        data = xhr.response;
        /* Add to 'Opgaver' to innerHTML */
        for (let i = 0; i < data.Opgaver.length; i++) {
            document.getElementById("side").innerHTML += `<div onclick="chooseAssignment('${data.Opgaver[i]}')">` + data.Opgaver[i] + `</div><br/><br/>`;
        }
        } else {
            console.log("Error");
        }    
    };
    // Check logged in cookie
    checkCookie();
    // Check last opened assignment cookie
    checkAssignmentCookie();
})

// Onclick events
window.onclick = function(event) {
    // Close modal if cliked outside of it
    if (event.target.matches(".modal") && (document.getElementById("modal1").style.display == "block" || document.getElementById("modal2").style.display == "block")) {
        modalClose(1);  
        modalClose(2);
    }
}

/* Modal open */
function modal(i) {
    document.getElementById(`modal${i}`).style.display = "block";
}

/* Modal close */
function modalClose(i) {
    document.getElementById(`modal${i}`).style.display = "none";
    document.getElementById("error").innerHTML = "";
}

/* Modal login form */
function login() {
    var username = document.getElementById("user").value;
    var password = document.getElementById("pwd").value;
    document.getElementById("user").value = "";
    document.getElementById("pwd").value = "";
    loginServer(username,password);
}

/* POST login */
function loginServer(username, password) {
    const xhr = new XMLHttpRequest();
    xhr.open("POST", "/login");
    xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    const body = JSON.stringify({
        user: username,
        pwd: password,
    });
    xhr.onload = () => {
        if (xhr.readyState == 4 && xhr.status == 201) {
          if (JSON.parse(xhr.responseText).status == "Accepted") {
            setCookie("username", username, 1);
            modalClose(1);
          } else {
            document.getElementById("error").innerHTML = "Login failed";
          }
        } else {
          console.log(`Error: ${xhr.status}`);
        }
        checkCookie();
    }
    xhr.send(body);
}

/* Choose assignment */
function chooseAssignment(i) {
    /* Change iframe src */
    document.getElementById("frame").src = `opgaver/${i}`;
    setCookie('assignment', i, 1);
}

// Signup function (POST request)
function signup() {
    var username = document.getElementById("user1").value;
    var password = document.getElementById("pwd1").value;

    document.getElementById("user1").value = "";
    document.getElementById("pwd1").value = "";

    const xhr = new XMLHttpRequest();
    xhr.open("POST", "/signup");
    xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    const body = JSON.stringify({
        user: username,
        pwd: password,
    });
    xhr.onload = () => {
        if (xhr.readyState == 4 && xhr.status == 201) {
          if (JSON.parse(xhr.responseText).status == "Accepted") {
            setCookie("username", username, 1);
            modalClose(2);
          } else {
            document.getElementById("error").innerHTML = "Login failed";
          }
        } else {
          console.log(`Error: ${xhr.status}`);
        }
        checkCookie();
    }
    xhr.send(body);
}

/* Cookie */
// Set any cookie
function setCookie(cookieName, cookieValue, cookieExp) {
    const date = new Date();
    date.setTime(date.getTime() + cookieExp * 1000 * 60 * 60 * 24);
    let expires = "expires="+date.toUTCString();
    document.cookie = cookieName + "=" + cookieValue + ";" + expires + ";path=/" 
}

// Get any cookie
function getCookie(cookieName) {    
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

// Check logged in cookie
function checkCookie() {
    let user = getCookie("username");
    if (user != "") {
        document.getElementById("log").innerHTML = "Logout";
        document.getElementById("log").onclick = logout;
    } else {
        document.getElementById("log").innerHTML = "Login";
        document.getElementById("log").setAttribute("onclick", "modal(1)");
    }
}

// Check assignment cookie
function checkAssignmentCookie() {
    let assign = getCookie("assignment");
    if (assign != "") {
        chooseAssignment(assign);
    }
}

// Logout function
function logout() {
    setCookie("username", "", 0);
    checkCookie();
}

