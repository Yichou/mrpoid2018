package com.mrpoid.apps;

import com.mrpoid.app.EmulatorActivity;
import com.mrpoid.app.EmulatorService;

public class AppService3 extends EmulatorService {
	static {
		EmulatorActivity.APP_ACTIVITY_NAME = "com.mrpoid.apps.AppActivity3";
		EmulatorActivity.APP_SERVICE_NAME = "com.mrpoid.apps.AppService3";
	}
}
