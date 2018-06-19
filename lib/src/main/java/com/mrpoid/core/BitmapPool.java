/*
 * Copyright (C) 2014 The Mrpoid Project
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
package com.mrpoid.core;

import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import com.edroid.common.utils.Logger;


/**
 * @author YYichou 2014-7-23
 *
 * 位图管理池，用于 底层绘图调用
 */
public class BitmapPool {
	static final Logger log = Logger.create(true, "BitmapPool");
	
	static Map<String, WeakReference<Bitmap>> map = new HashMap<String, WeakReference<Bitmap>>(12);
	static BitmapFactory.Options options = new BitmapFactory.Options();
	
	static {
		options.inPreferredConfig = Bitmap.Config.RGB_565;
	}
	
	
	public static void recyle() {
		Set<String> set = map.keySet();
		for(String key : set){
			WeakReference<Bitmap> r = map.get(key);
			Bitmap bmp = r!=null? r.get() : null;
			
			if(bmp != null && !bmp.isRecycled())
				bmp.recycle();
		}
		
		map.clear();
	}
	
	public static Bitmap getBitmap(String path) {
		WeakReference<Bitmap> wr = map.get(path);
		Bitmap bitmap = wr!=null? wr.get() : null;
		
		if(bitmap == null || bitmap.isRecycled()) {
			bitmap = BitmapFactory.decodeFile(path, options);
			map.put(path, new WeakReference<Bitmap>(bitmap));
			
			log.w("Cache bitmap:" + path + ", " + bitmap);
		}
		
		return bitmap;
	}
}
