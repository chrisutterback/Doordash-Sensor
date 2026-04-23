import 'package:flutter/material.dart';
import 'package:flutter_local_notifications/flutter_local_notifications.dart'; //local notifications
import 'package:permission_handler/permission_handler.dart'; //permission to have push notifications

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
  );

  // 5. Combine and Initialize
  const InitializationSettings initializationSettings = InitializationSettings(
    android: initializationSettingsAndroid,
    iOS: initializationSettingsIOS);

  await flutterLocalNotificationsPlugin.initialize(initializationSettings);
  await Permission.notification.request();
  runApp(const MyApp());
}
/*
 *  Helper function to pop up the alert
 */
Future<void> showTriggerAlert(String message) async {
  const NotificationDetails details = NotificationDetails(
    android: AndroidNotificationDetails('esp_channel', 'ESP32 Alerts', importance: Importance.max, priority: Priority.high),
  );
  await flutterLocalNotificationsPlugin.show(0, 'ESP32 Alert', message, details);
}
Future<void> _testEspAlert() async {
  // 1. Setup high priority so it actually 'pops up'
  const AndroidNotificationDetails androidDetails = AndroidNotificationDetails(
    'esp32_mock_channel', // Channel ID
    'ESP32 Alerts',       // Channel Name
    importance: Importance.max,
    priority: Priority.high,
  );


  const NotificationDetails platformDetails = NotificationDetails(
    android: androidDetails,
  );

  // 2. Show the notification
  await flutterLocalNotificationsPlugin.show(
      0,
      'ESP32 Alert!',
      'Mock trigger: Data received from WiFi.',
      platformDetails
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
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
      ),
      home: const MyHomePage(title: 'Flutter Demo Home Page'),
    );
  }
}


class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {

  void _incrementCounter() {
    setState(() {
      // This call to setState tells the Flutter framework that something has
      // changed in this State, which causes it to rerun the build method below
      // so that the display can reflect the updated values. If we changed
      // _counter without calling setState(), then the build method would not be
      // called again, and so nothing would appear to happen.
    });
  }

  @override
  Widget build(BuildContext context) {
    // This method is rerun every time setState is called, for instance as done
    // by the _incrementCounter method above.
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
    return Scaffold(
      appBar: AppBar(
        // TRY THIS: Try changing the color here to a specific color (to
        // Colors.amber, perhaps?) and trigger a hot reload to see the AppBar
        // change color while the other colors stay the same.
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        title: Text(widget.title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            ElevatedButton(
              onPressed: _testEspAlert,
              child: const Text('Simulate ESP32 Signal'),
            ),
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _incrementCounter,
        tooltip: 'Increment',
        child: const Icon(Icons.add),
      ),
    );
  }
}
