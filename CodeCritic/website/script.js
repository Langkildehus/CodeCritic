console.log("Jeg duer");
/* GET request - Opgaver */
const xhr = new XMLHttpRequest();
xhr.open("GET", "http://127.0.0.1/api/opgaver");
xhr.send();
console.log("Hej");
xhr.responseType = "json";
xhr.onload = () => {
    if (xhr.readyState == 4 && xhr.status == 200) {
        data = xhr.response;
        /* Add to 'Opgaver' to innerHTML */
        for (let i = 0; i < data.Opgaver.length; i++) {
            document.getElementById("side").innerHTML += `<div>` + data.Opgaver[i] + `</div><br/><br/>`;
        }
        console.log(data);
    } else {
        console.log("Error");
    }    
};

/* Login pop-up modal */
function modal() {
    document.getElementById("modal1").style.display = "block";
}

/* Modal close */
function modalClose() {
    document.getElementById("modal1").style.display = "none";
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
    xhr.open("POST", "http://127.0.0.1/api/opgaver");
    xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    const body = JSON.stringify({
        user: username,
        pwd: password,
    });
    xhr.onload = () => {
        if (xhr.readyState == 4 && xhr.status == 201) {
          console.log(JSON.parse(xhr.responseText));
        } else {
          console.log(`Error: ${xhr.status}`);
        }
      };
    xhr.send(body);
}