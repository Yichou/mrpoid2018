/*
 * Copyright (C) 2013 The Mrpoid Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.mrpoid.mrplist.app;

import com.mrpoid.app.EmulatorApplication;
import com.mrpoid.mrplist.moduls.FileType;
import com.mrpoid.mrplist.moduls.MyFavoriteManager;
import com.mrpoid.mrplist.moduls.PreferencesProvider;

/**
 * 
 * @author Yichou 2013-11-23
 * 
 */
public class MyApplication extends EmulatorApplication {
	
	
	
//	void runPlugin(PluginInfo info) {
//		try {
//			Class<?> cls = info.classLoader.loadClass("com.mrpoid.mrpliset.DemoPlugin");
//			
//			//Object init(Context context, String arg);
//			Method method = cls.getDeclaredMethod("init", Context.class, String.class);
//			Object ret = method.invoke(null, this, "hello");
//			System.out.println("插件初始化结果 " + ret);
//		} catch (Exception e) {
//			System.err.println("运行插件失败！" + e.getMessage());
//			e.printStackTrace();
//		}
//	}
//	
//	void loadPlugin() {
//		PluginLoader3.load(this, "mrpoid_demo", new PluginLoader3.PluginLoadCallback() {
//			
//			@Override
//			public void onUpdateVer(int oldVer, int newVer, String plgPath) {
//				System.out.println("插件有新版本，当前 " + oldVer + " 最新 " + newVer + "\n" + plgPath);
//			}
//			
//			@Override
//			public void onLoadSuccess(PluginInfo ret) {
//				System.out.println("插件加载成功，" + ret);
//			}
//			
//			@Override
//			public void onExist(int curVer, String plgPath) {
//				System.out.println("本地已存在插件");
//			}
//
//			@Override
//			public void onFailure(int code, String msg, Exception e) {
//				System.err.println("插件加载失败！" + code + " " + msg);
//			}
//		});
//	}
	
	@Override
	public void onCreate() {
		super.onCreate();
		
//		getPackageManager().re
		
//		loadPlugin();
		
		PreferencesProvider.load(this);
		
		FileType.loadIcons(getResources());
		
		MyFavoriteManager.getInstance().init(this);
		
//		MobclickAgent.setDebugMode(true);
	}
}
