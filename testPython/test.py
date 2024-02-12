import pyrebase

config = {
  "apiKey": "AIzaSyB5NElLq9i-qJJgqsYzyEaVlyLZaN0_RLo",
  "authDomain": "ir-signal.firebaseapp.com",
  "projectId": "ir-signal",
  "storageBucket": "ir-signal.appspot.com",
  "messagingSenderId": "419267853558",
  "appId": "1:419267853558:web:cf1f03cea9ea7b70b8c0d7",
  "databaseURL": "https://ir-signal-default-rtdb.asia-southeast1.firebasedatabase.app/"
}

firebase = pyrebase.initialize_app(config)

db = firebase.database()
# Fetch all data from the root
# all_data = db.child().get()

# # To fetch data from a specific path
# users = db.child("test").child("DistanceWidthTimingInfo").get()


# print(users.val())
# # Iterate through the data
# if users.each() is not None:
#     for user in users.each():
#         print(user.val()) # This is the value (data) for the user
# else:
#     print("No data found")

path = "0xB34C6E00"
a = db.child("IR-Signal").child(path).child("DistanceWidthTimingInfo").get()
b = db.child("IR-Signal").child(path).child("IRRawData").get()
c = db.child("IR-Signal").child(path).child("NumberOfBits").get()

db.child("IR-Signal-Choose").child("DistanceWidthTimingInfo").set(a.val())
db.child("IR-Signal-Choose").child("IRRawData").set(b.val())
db.child("IR-Signal-Choose").child("NumberOfBits").set(c.val())