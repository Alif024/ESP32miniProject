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
  var updates = {};
  updates['/Name'] = NameSignal;
  update(ref(database, rawDataText), updates);
  // console.log(rawDataText, NameSignal);
}


const starCountRef = ref(database, 'IR-Signal');
const RawDataActivate = ref(database, 'IR-Signal-Choose/IRRawData');
const NameActivate = ref(database, 'IR-Signal-Choose/Name');
document.addEventListener("DOMContentLoaded", function () {
  onValue(starCountRef, (snapshot) => {
    const data = snapshot.val();
    console.log(data);
    const tableData = document.getElementById("tableData");
    // const size = Object.keys(data).length;
    let count = 1;
    while (tableData.firstChild) {
      tableData.removeChild(tableData.firstChild);
    }
    Object.keys(data).forEach((key) => {
      const row = document.createElement("tr");
      const info = data[key];
      // Check if DistanceWidthTimingInfo is defined and is an array before calling join
      const distanceWidthTimingInfo = Array.isArray(info.DistanceWidthTimingInfo)
        ? info.DistanceWidthTimingInfo.join(", ")
        : "N/A";  // You can replace "N/A" with a default message or handling of your choice
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

    // Assuming you have a <select> element with id 'mySelect'
    const selectElement = document.getElementById('mySelect');

    // Clear existing options
    selectElement.innerHTML = '';

    // Add a disabled option as the first option
    const firstOption = document.createElement('option');
    firstOption.value = '';
    firstOption.disabled = true;
    firstOption.selected = true;
    firstOption.textContent = '-- เลือกสัญญาณที่ต้องการใช้ --';
    selectElement.appendChild(firstOption);

    // Add new options from the object
    Object.keys(data).forEach(key => {
      const option = document.createElement('option');
      option.value = key; // Using the key as the value for the option
      var textContent = data[key].IRRawData; // Using the 'Name' property as the text content for the option
      textContent = "0x" + textContent.toString(16).toUpperCase();
      option.textContent = textContent; // Using the 'Name' property as the text content for the option
      selectElement.appendChild(option);
    });
  });

  onValue(RawDataActivate, (snapshot) => {
    const RawData = snapshot.val();
    // console.log(data);
    const RawDataActivate = document.getElementById("RawDataActivate");
    RawDataActivate.innerText = "0x" + RawData.toString(16).toUpperCase();
  });

  onValue(NameActivate, (snapshot) => {
    const Name = snapshot.val();
    const ActivateSignalName = document.getElementById("ActivateSignalName");
    if (Name) {
      // console.log(data);
      ActivateSignalName.innerText = Name;
    } else {
      ActivateSignalName.innerText = "Copy Signal";
    }
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
  if (event.target == document.getElementById("popupChangeSignal")) {
    document.getElementById("popupChangeSignal").style.display = "none";
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
  }, 200);
  updateNameSignal(NumberSignal, NameSignal);
}


document.getElementById("btnChangeSignal").onclick = function () {
  document.getElementById("popupChangeSignal").style.display = "flex";
}

document.querySelector(".closeChangeSignal").onclick = function () {
  document.getElementById("popupChangeSignal").style.display = "none";
}

const submitBtnChangeSignalBtn = document.getElementById('submitChangeSignalBtn');
submitBtnChangeSignalBtn.onclick = () => {
  // Get the select element by its ID
  const selectElement = document.getElementById('mySelect');
  // Get the value of the selected option
  const selectedValue = selectElement.value;
  // Log the value to the console
  // console.log(selectedValue);
  document.getElementById("popupChangeSignal").style.display = "none";

  const path = ref(database, "IR-Signal/" + selectedValue);
  onValue(path, (snapshot) => {
    const data = snapshot.val();
    handleData(data);
  });

  function handleData(data) {
    var DistanceWidthTimingInfo = data.DistanceWidthTimingInfo;
    var IRRawData = data.IRRawData;
    var NumberOfBits = data.NumberOfBits;
    var Name = data.Name;

    // Create an object with the data you always want to set
    var dataToSet = {
      DistanceWidthTimingInfo: DistanceWidthTimingInfo,
      IRRawData: IRRawData,
      NumberOfBits: NumberOfBits
    };

    // Only add Name to the object if it exists
    if (Name) {
      dataToSet.Name = Name;
    }

    set(ref(database, "IR-Signal-Choose"), dataToSet);
  }
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

  $('#mySelect').select2();
});