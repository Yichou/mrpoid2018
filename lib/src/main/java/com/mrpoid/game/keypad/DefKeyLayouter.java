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
package com.mrpoid.game.keypad;

import com.mrpoid.MrpoidMain;
import com.mrpoid.R;
import com.mrpoid.core.MrDefines;
import com.mrpoid.core.MrpoidSettings;
import com.mrpoid.game.engine.ActorGroup;
import com.mrpoid.game.keypad.Keypad.KeyLayouter;


/**
 * 默认键盘布局器
 * 
 * @author YYichou 2014-6-26
 *
 */
public class DefKeyLayouter implements KeyLayouter {
	private int viewW, viewH;
	
	
	protected void createLand(Keypad keypad) { //横屏
		boolean dpadAtLeft = MrpoidSettings.dpadAtLeft;
		boolean smpleMode = mode == 2;
		
		final float base = MrpoidMain.getResources().getDimension(R.dimen.dp1);
		int numW = Math.round(base*45), numH = Math.round(base*30),
				numM = Math.round(base*8), softW = Math.round(base*45),
				softH = Math.round(base*30);
		int padW = Math.round(base*40), padH = Math.round(base*30), 
				padR = Math.round(base*20), padM = Math.round(base*8);
		
		DPad pad = new DPad(keypad);
		pad.setSize(padW, padH, padR, padM);
		pad.setClickCallback(keypad);
		root.addChild(pad);
		
		// 确定
		TextButton btnOk = new TextButton(keypad,
				"左软", 
				MrDefines.MR_KEY_SOFTLEFT, 0, 0,
				keypad);
		btnOk.setSize(softW, softH);
		root.addChild(btnOk);
		
		// 返回
		TextButton btnCancel = new TextButton(keypad,
				"右软", 
				MrDefines.MR_KEY_SOFTRIGHT, 0, 0,
				keypad);
		btnCancel.setSize(softW, softH);
		root.addChild(btnCancel);
		
		ActorGroup numGroup = null;
		if(!smpleMode){
			numGroup = new ActorGroup(keypad);
			root.addChild(numGroup);
			
			int x = 0;
			int y = 0;
			
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 3; j++) {
					TextButton button = new TextButton(keypad,
							titles[i * 3 + j], 
							ids[i * 3 + j], x, y,
							keypad);
					button.setSize(numW, numH);
					numGroup.addChild(button);
					
					x += numW + numM;
				}
				x = 0;
				y += numH + numM;
			}
		}
		
		int y0 = viewH - (numH + numM) * 4;
		if (dpadAtLeft) {
			pad.setPosition(numM, (viewH - pad.getH() - numM));

			int x0 = viewW - (numW + numM) * 3;

			if(!smpleMode) 
				numGroup.setPosition(x0, y0);
		} else {
			pad.setPosition((viewW - pad.getW() - numM), (viewH - pad.getH() - numM));

			if(!smpleMode) 
				numGroup.setPosition(numM, y0);
		}

		btnOk.setPosition(pad.getX(), pad.getY() - btnOk.getH() - numM);
		btnCancel.setPosition(pad.getR() - btnCancel.getW() - numM, pad.getY() - btnCancel.getH() - numM);
	}
	
	protected void createXhidp(Keypad keypad) {
		boolean dpadAtLeft = MrpoidSettings.dpadAtLeft;
		boolean smpleMode = mode == 2;
		
		final float base = MrpoidMain.getResources().getDimension(R.dimen.dp1);
		int numW = Math.round(base*45), numH = Math.round(base*30),
				numM = Math.round(base*8), softW = Math.round(base*45),
				softH = Math.round(base*30);
		int padW = Math.round(base*40), padH = Math.round(base*30), 
				padR = Math.round(base*20), padM = Math.round(base*8);

		
		DPad pad = new DPad(keypad);
		pad.setSize(padW, padH, padR, padM);
		pad.setClickCallback(keypad);
		root.addChild(pad);
		
		// 确定
		TextButton btnOk = new TextButton(keypad,
				"左软", 
				MrDefines.MR_KEY_SOFTLEFT, 0, 0,
				keypad);
		btnOk.setSize(softW, softH);
		root.addChild(btnOk);
		
		// 返回
		TextButton btnCancel = new TextButton(keypad,
				"右软", 
				MrDefines.MR_KEY_SOFTRIGHT, 0, 0,
				keypad);
		btnCancel.setSize(softW, softH);
		root.addChild(btnCancel);
		
		if (smpleMode) {
			pad.setPosition((viewW - pad.getW())/2, (viewH - pad.getH() - numM));
			
			btnOk.setPosition(numM*2, (viewH - btnOk.getH() - 2*numM));
			btnCancel.setPosition((viewW - btnCancel.getW() - 2*numM), btnOk.getY());
			
			return;
		}
		
		ActorGroup numGroup = new ActorGroup(keypad);
		root.addChild(numGroup);
		int x = 0;
		int y = 0;
		
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 3; j++) {
				TextButton button = new TextButton(keypad,
						titles[i * 3 + j], 
						ids[i * 3 + j], x, y,
						keypad);
				button.setSize(numW, numH);
				numGroup.addChild(button);
				
				x += numW + numM;
			}
			x = 0;
			y += numH + numM;
		}
		
		{
			pad.setPosition(dpadAtLeft? numM : (viewW - pad.getW() - numM), (viewH - pad.getH() - numM));
			
			int x0;
			
			x0 = dpadAtLeft? (viewW - (numW+ numM)*3) : numM;
			y = viewH - (numM + numH)*4;
			numGroup.setPosition(x0, y);
			
			btnOk.setPosition(pad.getX(), y);
			btnCancel.setPosition(pad.getR() - numW, y);
		}
	}
	
	int mode;
	ActorGroup root;
	
	@Override
	public void layout(Keypad keypad, ActorGroup root, boolean landscape, int mode) {
		this.mode = mode;
		this.root = root;
		
		if(landscape)
			createLand(keypad);
		else 
			createXhidp(keypad);
	}
	
	@Override
	public void setSize(int neww, int newh) {
		viewW = neww;
		viewH = newh;
	}

	private static final String[] titles = {
		"1", "2", "3",
		"4", "5", "6", 
		"7", "8", "9", 
		"*", "0", "#"
	};

	private static final int[] ids = {
		MrDefines.MR_KEY_1, MrDefines.MR_KEY_2, MrDefines.MR_KEY_3,
		MrDefines.MR_KEY_4, MrDefines.MR_KEY_5, MrDefines.MR_KEY_6, 
		MrDefines.MR_KEY_7, MrDefines.MR_KEY_8, MrDefines.MR_KEY_9, 
		MrDefines.MR_KEY_STAR, MrDefines.MR_KEY_0, MrDefines.MR_KEY_POUND
	};
}
