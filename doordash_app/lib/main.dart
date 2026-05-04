import 'package:flutter/material.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

// 1. Create a global instance
final FlutterLocalNotificationsPlugin flutterLocalNotificationsPlugin =
FlutterLocalNotificationsPlugin();

void main() async {
  WidgetsFlutterBinding.ensureInitialized();

  const AndroidInitializationSettings initializationSettingsAndroid =
  AndroidInitializationSettings('@mipmap/ic_launcher');

  const DarwinInitializationSettings initializationSettingsIOS =
  DarwinInitializationSettings(
    requestAlertPermission: true,
    requestBadgePermission: true,
    requestSoundPermission: true,
    defaultPresentAlert: true,
    defaultPresentBadge: true,
    defaultPresentSound: true,
    defaultPresentBanner: true,
    defaultPresentList: true,
  );

  const InitializationSettings initializationSettings = InitializationSettings(
      android: initializationSettingsAndroid,
      iOS: initializationSettingsIOS);

  await flutterLocalNotificationsPlugin.initialize(initializationSettings);

  await flutterLocalNotificationsPlugin
      .resolvePlatformSpecificImplementation<
      IOSFlutterLocalNotificationsPlugin>()
      ?.requestPermissions(
    alert: true,
    badge: true,
    sound: true,
  );

  runApp(const MyApp());
}

Future<void> showTriggerAlert(String message) async {
  const NotificationDetails details = NotificationDetails(
    android: AndroidNotificationDetails('esp_channel', 'ESP32 Alerts',
        importance: Importance.max, priority: Priority.high),
    iOS: DarwinNotificationDetails(
      presentAlert: true,
      presentBadge: true,
      presentSound: true,
    ),
  );
  await flutterLocalNotificationsPlugin.show(
    DateTime.now().millisecondsSinceEpoch ~/ 1000,
    'ESP32 Alert',
    message,
    details,
  );
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Delivery Tracker',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
      ),
      home: const MyHomePage(title: 'Delivery Tracker'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});
  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {

  // Replace with your ESP32's IP from Serial Monitor
  final _channel = WebSocketChannel.connect(
    Uri.parse('ws://172.20.10.12:81'),
  );


  String _esp32Data = "Waiting for ESP32...";

  List<Map<String, dynamic>> deliveries = [
    {"name": "DoorDash", "delivered": false, "image": "assets/images/DoorDash.png"},
    {"name": "Uber Eats", "delivered": false, "image": "assets/images/uberEats.png"},
    {"name": "Amazon", "delivered": false, "image": "assets/images/Amazon.png"},
  ];

  @override
  void dispose() {
    _channel.sink.close();
    super.dispose();
  }

  bool _isConnected = false;

  @override
  void initState() {
    super.initState();

    _channel.stream.listen(
          (message) {
        var parts = message.toString().split(':');
        setState(() {
          _isConnected = true;
          _esp32Data = message.toString();
        });

        if (parts[0] == "DELIVERED") {
          showTriggerAlert("${parts[1]} has been delivered!");
        }
      },
      onError: (error) {
        setState(() => _isConnected = false);
      },
      onDone: () {
        setState(() => _isConnected = false);
      },
    );
  }
  // @override
  // void initState() {
  //   super.initState();
  //
  //   // Listen for ESP32 messages
  //   _channel.stream.listen((message) {
  //     var parts = message.toString().split(':');
  //     setState(() {
  //       _esp32Data = message.toString();
  //     });
  //
  //     // If ESP32 sends "DELIVERED:DoorDash" it triggers the notification
  //     if (parts[0] == "DELIVERED") {
  //       showTriggerAlert("${parts[1]} has been delivered!");
  //     }
  //   });
  // }

  void markDelivered(int index) {
    setState(() {
      deliveries[index]["delivered"] = true;
    });
    showTriggerAlert("${deliveries[index]["name"]} has been delivered!");
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Delivery Tracker"),
        backgroundColor: Colors.blue,
      ),
      // Shows what the ESP32 is currently sending at the top
      body: Column(
        children: [
          Container(
            padding: EdgeInsets.all(10),
            color: Colors.grey[200],
            child: Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Icon(Icons.sensors, color: Colors.blue),
                SizedBox(width: 8),
                Text("ESP32: $_esp32Data",
                    style: TextStyle(fontWeight: FontWeight.bold)),
              ],
            ),
          ),
          Expanded(
            child: ListView.builder(
              itemCount: deliveries.length,
              itemBuilder: (context, index) {
                final item = deliveries[index];

                return Card(
                  margin: EdgeInsets.all(10),
                  child: ListTile(
                    leading: item["delivered"]
                        ? Icon(Icons.check_circle, color: Colors.green)
                        : Image.asset(
                      item["image"],
                      width: 40,
                      height: 40,
                    ),
                    title: Text(item["name"]),
                    subtitle: Text(
                      item["delivered"] ? "Delivered" : "Pending",
                    ),
                    trailing: ElevatedButton(
                      onPressed: item["delivered"]
                          ? null
                          : () => markDelivered(index),
                      child: Text("Deliver"),
                    ),
                  ),
                );
              },
            ),
          ),
        ],
      ),
    );
  }
}