package com.mrpoid.mrplist.app;

import com.mrpoid.mrpliset.R;
import com.mrpoid.utils.UmengUtils;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.Window;
import android.view.WindowManager;

public class WelcomeActivity extends AppCompatActivity {

	void go() {
		SharedPreferences sp = getPreferences(0);
		if (!sp.getBoolean("showLogo", true)) {
			startActivity(new Intent(WelcomeActivity.this, HomeActivity.class));
			finish();
			return;
		}

		sp.edit().putBoolean("showLogo", false).commit();

		new Handler().postDelayed(new Runnable() {

			@Override
			public void run() {
				startActivity(new Intent(WelcomeActivity.this, HomeActivity.class));
				finish();
			}
		}, 3000);
	}
	
	void gp3() {
        String[] perms = { Manifest.permission.READ_PHONE_STATE};
        if (!isgen(perms))
            gen(perms, 3);
        else
            go();
	}

	void gp1() {
	    String[] perms = { Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE};
		if (!isgen(perms))
		    gen(perms, 1);
		else
		    gp2();
	}

	void gp2() {
	    String[] perms = {Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.ACCESS_FINE_LOCATION};
        if (!isgen(perms))
            gen(perms, 2);
        else
            gp3();
	}

	void gen(String[] perms, int code) {
        ActivityCompat.requestPermissions(this, perms, code);
    }

	boolean isgen(String[] perms) {
	    for (String perm : perms) {
            if (ContextCompat.checkSelfPermission(this, perm) != PackageManager.PERMISSION_GRANTED)
                return false;
        }
        return true;
    }

	boolean isok(int[] grantResults) {
		for (int grant : grantResults) {
			if (grant != PackageManager.PERMISSION_GRANTED) {
				return false;
			}
		}

		return true;
	}

	@Override
	public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
		switch (requestCode) {
			case 1: {
				if(isok(grantResults)) {
					gp2();
				} else {
					gp1();
				}
				break;
			}

			case 2: {
				if(isok(grantResults)) {
					gp3();
				} else {
					gp2();
				}
				break;
			}
			case 3: {
				if(isok(grantResults)) {
                    go();
				} else {
					gp3();
				}
				break;
			}
		}

		super.onRequestPermissionsResult(requestCode, permissions, grantResults);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

//		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);

		setContentView(R.layout.activity_welcome);

		boolean ok = true;

		if (Build.VERSION.SDK_INT >= 23) {
			gp1();
		} else
		    go();
	}

	@Override
	protected void onPause() {
		UmengUtils.onPause(this);

		super.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();

		UmengUtils.onResume(this);
	}
}
