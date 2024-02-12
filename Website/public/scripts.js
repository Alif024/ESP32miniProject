import { initializeApp } from "https://www.gstatic.com/firebasejs/10.8.0/firebase-app.js";
import { getDatabase, ref, onValue, set, update } from "https://www.gstatic.com/firebasejs/10.8.0/firebase-database.js";
const firebaseConfig = {
  apiKey: "AIzaSyB5NElLq9i-qJJgqsYzyEaVlyLZaN0_RLo",
  authDomain: "ir-signal.firebaseapp.com",
  projectId: "ir-signal",
  storageBucket: "ir-signal.appspot.com",
  messagingSenderId: "419267853558",
  appId: "1:419267853558:web:cf1f03cea9ea7b70b8c0d7",
  databaseURL: "https://ir-signal-default-rtdb.asia-southeast1.firebasedatabase.app/"
};
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

// test 1 value
// onValue(starCountRef, (snapshot) => {
//   const data = snapshot.val();
//   const irRawDataHex = data["0x7B34ED12"].IRRawData.toString(16).toUpperCase();
//   console.log(data["0x7B34ED12"].Name);
//   console.log("0x" + irRawDataHex);
// });


function updateNameSignal(NumberSignal, NameSignal) {
  var rawDataText = "IR-Signal/" + document.getElementById("Raw-Data-" + NumberSignal).textContent;
  update(ref(database, rawDataText), {
    Name: NameSignal
  });
  console.log(rawDataText);
}


const starCountRef = ref(database, 'IR-Signal');
document.addEventListener("DOMContentLoaded", function () {
  onValue(starCountRef, (snapshot) => {
    const data = snapshot.val();
    // console.log(data);
    const tableData = document.getElementById("tableData");
    const size = Object.keys(data).length;
    let count = 1;
    while (tableData.firstChild) {
      tableData.removeChild(tableData.firstChild);
    }
    Object.keys(data).forEach((key) => {
      const row = document.createElement("tr");
      const info = data[key];
      const distanceWidthTimingInfo = info.DistanceWidthTimingInfo.join(", ");
      row.innerHTML = `
        <td>${count}</td>
        <td id="Raw-Data-${count}">${key}</td>
        <td class="no-txt-center">${distanceWidthTimingInfo}</td>
        <td>${info.NumberOfBits}</td>
        <td id="NameNo.${count}">${info.Name || ''}</td>
      `;
      tableData.appendChild(row);
      count++;
    });
  });
});

// รับองค์ประกอบต่างๆ
var modal = document.getElementById("myModal");
var btn = document.getElementById("openModalBtn");
var span = document.getElementsByClassName("close")[0];
var submitBtn = document.getElementById("submitBtn");

// เมื่อผู้ใช้คลิกปุ่ม, เปิด modal 
btn.onclick = function () {
  btn.classList.add('clicked'); // เพิ่มคลาสเมื่อคลิก
  setTimeout(() => {
    btn.classList.remove('clicked'); // ลบคลาสหลังจาก 200 ms
    modal.style.display = "flex";
  }, 200);
}

// เมื่อผู้ใช้คลิกที่ (x), ปิด modal
span.onclick = function () {
  modal.style.display = "none";
}

// เมื่อผู้ใช้คลิกนอก modal, ปิดมัน
window.onclick = function (event) {
  if (event.target == modal) {
    modal.style.display = "none";
  }
}

// จัดการกับการส่งข้อมูล
submitBtn.onclick = function () {
  var NumberSignal = document.getElementById("NumberSignal").value;
  var NameSignal = document.getElementById("NameSignal").value;

  // ทำอะไรกับค่า value1 และ value2
  submitBtn.classList.add('clicked'); // เพิ่มคลาสเมื่อคลิก
  setTimeout(() => {
    submitBtn.classList.remove('clicked'); // ลบคลาสหลังจาก 200 ms
    modal.style.display = "none";
    updateNameSignal(NumberSignal, NameSignal);
  }, 200);
}


// test send data
// function writeUserData(IR_Signal, DistanceWidthTimingInfo, NumberOfBits) {
//   set(ref(database, "IR-Signal/0x" + IR_Signal.toString(16).toUpperCase()), {
//     IRRawData: IR_Signal,
//     DistanceWidthTimingInfo: DistanceWidthTimingInfo,
//     NumberOfBits: NumberOfBits
//   });
// }
// writeUserData(0x7000000520C, [9050, 4550, 600, 1650, 600, 500], 44);


$(document).ready(function () {
  // เมื่อเอกสารพร้อม
  sortTable();

  function sortTable() {
    var rows = $('#tableData tr').get();

    rows.sort(function (a, b) {
      var A = parseInt($(a).children('td').first().text()); // ค่า No. ของแถว a
      var B = parseInt($(b).children('td').first().text()); // ค่า No. ของแถว b
      return A - B;
    });

    $.each(rows, function (index, row) {
      $('#tableData').append(row); // นำแถวที่เรียงลำดับแล้วกลับไปยัง tbody
    });
  }

  $("#myInput").on("keyup", function () {
    var value = $(this).val().toLowerCase();
    $("#tableData tr").filter(function () {
      $(this).toggle($(this).text().toLowerCase().indexOf(value) > -1);
    });
  });
});