import 'package:flutter/material.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';

// 1. Create a global instance
final FlutterLocalNotificationsPlugin flutterLocalNotificationsPlugin =
FlutterLocalNotificationsPlugin();

void main() async {
  // 2. Required for any async setup in main
  WidgetsFlutterBinding.ensureInitialized();

  // 3. Android-specific settings (icon must exist in android/app/src/main/res/drawable)
  const AndroidInitializationSettings initializationSettingsAndroid =
  AndroidInitializationSettings('@mipmap/ic_launcher');

  // 4. iOS-specific settings
  const DarwinInitializationSettings initializationSettingsIOS =
  DarwinInitializationSettings(
    requestAlertPermission: true,
    requestBadgePermission: true,
    requestSoundPermission: true,
    defaultPresentAlert: true,
    defaultPresentBadge: true,
    defaultPresentSound: true,
  );

  // 5. Combine and Initialize
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
/**
 *  Helper function to pop up the alert
 */
Future<void> showTriggerAlert(String message) async {
  const NotificationDetails details = NotificationDetails(
    android: AndroidNotificationDetails('esp_channel', 'ESP32 Alerts', importance: Importance.max, priority: Priority.high), iOS: DarwinNotificationDetails(presentAlert: true,
    presentBadge: true,
    presentSound: true,),
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

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        // This is the theme of your application.
        //
        // TRY THIS: Try running your application with "flutter run". You'll see
        // the application has a purple toolbar. Then, without quitting the app,
        // try changing the seedColor in the colorScheme below to Colors.green
        // and then invoke "hot reload" (save your changes or press the "hot
        // reload" button in a Flutter-supported IDE, or press "r" if you used
        // the command line to start the app).
        //
        // Notice that the counter didn't reset back to zero; the application
        // state is not lost during the reload. To reset the state, use hot
        // restart instead.
        //
        // This works for code too, not just values: Most code changes can be
        // tested with just a hot reload.
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
      ),
      home: const MyHomePage(title: 'Flutter Demo Home Page'),
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
  List<Map<String, dynamic>> deliveries = [
    {"name": "DoorDash", "delivered": false, "image": "assets/images/DoorDash.png"},
    {"name": "Uber Eats", "delivered": false, "image": "assets/images/uberEats.png"},
    {"name": "Amazon", "delivered": false, "image": "assets/images/Amazon.png"},
  ];

  void markDelivered(int index) {
    setState(() {
      deliveries[index]["delivered"] = true;
    });

    // 🔔 Use your friend's notification function
    print("NOTIFICATION TRIGGERED");
    showTriggerAlert("${deliveries[index]["name"]} delivered!");
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Delivery Tracker"),
        backgroundColor: Colors.blue,
      ),
      body: ListView.builder(
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
    );
  }
}

