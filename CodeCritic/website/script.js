/* GET request - Opgaver */
const xhr = new XMLHttpRequest();
xhr.open("GET", "http://127.0.0.1/opgaver");
xhr.send();
console.log("Hej");
xhr.responseType = "json";
xhr.onload = () => {
    if (xhr.readyState == 4 && xhr.status == 200) {
        data = xhr.response;
        /* Add to 'Opgaver' to innerHTML */
        for (let i = 0; i < data.Opgaver.length; i++) {
            document.getElementById("side").innerHTML += `<div onclick="chooseAssignment('${data.Opgaver[i]}')">` + data.Opgaver[i] + `</div><br/><br/>`;
        }
        console.log(data);
    } else {
        console.log("Error");
    }    
};
window.addEventListener("load", () => {
    checkCookie();
    checkAssignmentCookie();
})

/* Login pop-up modal */
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
    console.log(username);
    console.log(password);
    loginServer(username,password);
}

/* POST login */
function loginServer(username, password) {
    const xhr = new XMLHttpRequest();
    xhr.open("POST", "http://127.0.0.1/login");
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
            console.log("No");
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

function signup() {
    var username = document.getElementById("user1").value;
    var password = document.getElementById("pwd1").value;

    document.getElementById("user1").value = "";
    document.getElementById("pwd1").value = "";

    const xhr = new XMLHttpRequest();
    xhr.open("POST", "http://127.0.0.1/signup");
    xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    const body = JSON.stringify({
        user: username,
        pwd: password,
    });

    console.log(body);
    xhr.onload = () => {
        if (xhr.readyState == 4 && xhr.status == 201) {
          if (JSON.parse(xhr.responseText).status == "Accepted") {
            setCookie("username", username, 1);
            modalClose(2);
          } else {
            console.log("No");
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

function setCookie(cookieName, cookieValue, cookieExp) {
    const date = new Date();
    date.setTime(date.getTime() + cookieExp * 1000 * 60 * 60 * 24);
    let expires = "expires="+date.toUTCString();
    document.cookie = cookieName + "=" + cookieValue + ";" + expires + ";path=/" 
}

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

function checkAssignmentCookie() {
    let assign = getCookie("assignment");
    if (assign != "") {
        chooseAssignment(assign);
    }
}

function logout() {
    console.log("Logoutfeg");
    setCookie("username", "", 0);
    checkCookie();
}

