package com.mrpoid.apps;

import com.mrpoid.app.EmulatorActivity;
import com.mrpoid.app.EmulatorService;

public class AppService1 extends EmulatorService {
	static {
		EmulatorActivity.APP_ACTIVITY_NAME = "com.mrpoid.apps.AppActivity1";
		EmulatorActivity.APP_SERVICE_NAME = "com.mrpoid.apps.AppService2";
	}
}
