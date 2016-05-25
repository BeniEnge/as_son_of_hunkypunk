/*
	Copyright © 2009 Rafał Rzepecki <divided.mind@gmail.com>

	This file is part of Hunky Punk.

    Hunky Punk is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Hunky Punk is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Hunky Punk.  If not, see <http://www.gnu.org/licenses/>.
*/


/* Comments by: JPDOB-Team, Dimitar Garkov
 *               University of Constance, 2016
 *
 * Copyright: The following version of 'Son of Hunky Punk' obeys the 
 *            GNU General Public License. Since it is clearly stated in  
 *            5. c), 'Son of Hunky Punk' obeys only the GNU GPL v3.
 *            All modifications are (to be) done according to 
 *            the GNU General Public License version 3, paragraph 5.
 *            
 *            All contributors as of GNU GPL are in a way stated.
 *
 *
 * The Interpreter Activity takes care for the interactions with the single games.
 * It loads stories and checks their endings. The setFont() bug is fixed and 
 * now works correctly.
 * It then accepts input and *interprets*, i.e. passes it to the story. This,
 * however, is implemented not here, but only loaded.
 */


package org.andglk.hunkypunk;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;

import org.andglk.glk.FileRef;
import org.andglk.glk.FileStream;
import org.andglk.glk.Glk;
import org.andglk.glk.Window;
import org.andglk.glk.TextBufferWindow;
import org.andglk.hunkypunk.HunkyPunk;
import org.andglk.hunkypunk.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.os.Parcelable;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

//Debug
import android.widget.Toast;
//added
import android.view.View;
import android.view.Gravity;
//import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout.LayoutParams;
import android.view.LayoutInflater;
import android.content.Context; 
import android.widget.FrameLayout;
import android.view.View.OnClickListener;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.RelativeLayout;


public class Interpreter extends Activity {
    private static final String TAG = "hunkypunk.Interpreter";
	private Glk glk;
	private File mDataDir;
	private ImageButton compassNorth;
	private ImageButton compassEast;
	private ImageButton compassSouth;
	private ImageButton compassWest;

	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	System.loadLibrary("andglk-loader");

    	setTheme(R.style.theme);
		setFont();

        Intent i = getIntent();
        Uri uri = i.getData();
        String terp = i.getStringExtra("terp");
        String ifid = i.getStringExtra("ifid");
        mDataDir = HunkyPunk.getGameDataDir(uri, ifid);
        File saveDir = new File(mDataDir, "savegames");
        saveDir.mkdir();

		glk = new Glk(this);

        setContentView(glk.getView());
	/*added*/
	LayoutInflater compassInflater = (LayoutInflater) getSystemService (Context.LAYOUT_INFLATER_SERVICE);
    	compassInflater = LayoutInflater.from(getApplicationContext());
	View mView = compassInflater.inflate(R.layout.compass_button, null);
	FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        lp.gravity=Gravity.CENTER;
        addContentView(mView, lp);
	/*/added*/
		glk.setAutoSave(getBookmark(), 0);
        glk.setSaveDir(saveDir);
        glk.setTranscriptDir(Paths.transcriptDirectory()); // there goes separation, and so cheaply...

		ArrayList<String> args = new ArrayList<String>();
		args.add(getFilesDir()+"/../lib/lib"+terp+".so");
		if(terp.compareTo("tads")==0 && getBookmark().exists()) {
			args.add("-r");
			args.add(getBookmark().getAbsolutePath());
		}
		args.add(uri.getPath());

		String arga[] = new String[args.size()];
		glk.setArguments(args.toArray(arga));

        super.onCreate(savedInstanceState);

		// dead code, doesn't work
		// TODO: remove all the Parcelable/SavedState objects and onRestoreInstanceState code in Windows 
		//if (savedInstanceState != null)
        //	restore(savedInstanceState.getParcelableArrayList("windowStates"));
        //else 

		if (i.getBooleanExtra("loadBookmark", false) || savedInstanceState != null) {
			/* either the user's intent is to restore from bookmark,
			   or the OS has killed our app and is now restoring state */
        	loadBookmark();
		}
	/*added*/
	compassNorth = (ImageButton) findViewById(R.id.compass_north);
	compassNorth.setOnClickListener(new OnClickListener(){
		@Override
		public void onClick(View view) {
			//debugging msg
 			Toast.makeText(getApplicationContext(), "Go North!",Toast.LENGTH_LONG).show();
		}
	});
	compassEast = (ImageButton) findViewById(R.id.compass_east);
	compassEast.setOnClickListener(new OnClickListener(){
		@Override
		public void onClick(View view) {
		//debugging msg
 		Toast.makeText(getApplicationContext(), "Go East!",Toast.LENGTH_LONG).show();
		}
	});
	compassSouth = (ImageButton) findViewById(R.id.compass_south);
	compassSouth.setOnClickListener(new OnClickListener(){
		@Override
		public void onClick(View view) {
			//debugging msg
 			Toast.makeText(getApplicationContext(), "Go South!",Toast.LENGTH_LONG).show();
		}
	});
	compassWest = (ImageButton) findViewById(R.id.compass_west);
	compassWest.setOnClickListener(new OnClickListener(){
		@Override
		public void onClick(View view) {
		//debugging msg
 		Toast.makeText(getApplicationContext(), "Go West!",Toast.LENGTH_LONG).show();
		}
	});
	/*/added*/
    	glk.start();
    }

	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = new MenuInflater(getApplication());
		inflater.inflate(R.layout.menu_main, menu);
		return true;
	}

	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item) {
		Intent intent;
		switch (item.getNumericShortcut()) {
		case '1':
			intent = new Intent(this, PreferencesActivity.class);
			startActivity(intent);
			break;
		case '2':
			AlertDialog builder;
			try {
				builder = AboutDialogBuilder.show(this);
			} catch (NameNotFoundException e) {
				e.printStackTrace();
			}
			break;
		}
		return super.onMenuItemSelected(featureId, item);
	}

    private void loadBookmark() {
    	final File f = getBookmark(); 
    	if (!f.exists())
    		return;

    	final File ws = getWindowStates();
    	if (ws.exists()) try {
    		final FileInputStream fis = new FileInputStream(ws);
    		if (fis != null) {
    			final ObjectInputStream ois = new ObjectInputStream(fis);
		    	Glk.getInstance().onSelect(new Runnable() {
		    		@Override
		    		public void run() {
		    	    	Glk.getInstance().waitForUi(new Runnable() {
		    	    		public void run() {
				    	    	Window w = null;
				    	    	try {
				    	    		while ((w = Window.iterate(w)) != null) {
				    	    			w.readState(ois);
										w.flush();
									}
									ois.close();
					    	    	fis.close();
								} catch (IOException e) {
									Log.w(TAG, "error while reading window states", e);
								}
		    	    		}
		    	    	});
		    		}
		    	});
			}
    	} catch (IOException e) {
			Log.e(TAG, "error while opening window state stream", e);
    	}
	}

	/* dead code, doesn't work
	// TODO: remove all the Parcelable/SavedState objects and onRestoreInstanceState code in Windows
	private void restore(final ArrayList<Parcelable> windowStates) {
    	final File f = getBookmark(); 
    	if (!f.exists())
    		return;

    	Glk.getInstance().onSelect(new Runnable() {
    		@Override
    		public void run() {
    	    	if (windowStates != null)
	    	    	Glk.getInstance().waitForUi(new Runnable() {
	    	    		public void run() {
			    	    	Window w = null;
			    	    	for (Parcelable p : windowStates)
			    	    		if ((w = Window.iterate(w)) != null) {
			    	    			w.restoreInstanceState(p);
									w.flush();
								}
			    	    		else
			    	    			break;
	    	    		}
	    	    	});
    		}
    	});
    }
	*/
	
	@Override
    public void onConfigurationChanged(Configuration newConfig) {
    	super.onConfigurationChanged(newConfig);
    	glk.onConfigurationChanged(newConfig);
    }

    @Override
    protected void onResume() {
    	super.onResume();

		if (setFont()) glk.getView().invalidate();
	}

	/* Changing font fixed with a workaround solution. Apparently, 
 	 * all that was needed was setting the returned value to the 
	 * default value in TextBufferWindow and performing the other changes there.
	 * Path option is eliminated. Later, would be thought of letting the user 
	 * upload downloaded fonts, but for now the available fonts are fixed. More
	 * fonts are to be added.
	 */

	private boolean setFont() {
		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
		
		//TODO: changing font is broken (text overflows the view) |:fixed:| 
		
		//String fontFolder = prefs.getString("fontFolderPath", "/sdcard/Fonts");
		//String fontFile = prefs.getString("fontFileName", "Droid Serif");
		//String fontPath;
		//File fonts = new File(fontFolder, fontFile);


		int fontSize = 16;
		try{
			fontSize = Integer.parseInt(prefs.getString("fontSize", Integer.toString(fontSize)));
		}catch(Exception e){}

		String fontName = prefs.getString("fontFileName", "Droid Serif"); //returns the Svalue in "fontfileName"-preference and otherwise "DSerif" 
		//debugging msg
		if (TextBufferWindow.DefaultFontName == null)
     				Toast.makeText(getApplicationContext(), fontName,Toast.LENGTH_LONG).show();

		TextBufferWindow.DefaultFontName = fontName;

		if (TextBufferWindow.DefaultFontSize != fontSize) {
		/*dead code*/
		//	TextBufferWindow.DefaultFontName == null) {
		//	|| TextBufferWindow.DefaultFontPath.compareTo(fontPath)!=0) {

			//TextBufferWindow.DefaultFontName = fontName; //TODO: try again here in the IF
			TextBufferWindow.DefaultFontSize = fontSize;
			return true;
		}
		return false;
	}
    
    @Override
    protected void onPause() {
    	super.onPause();
		if (glk.isAlive()) {
			glk.postAutoSaveEvent(getBookmark().getAbsolutePath());

			final File ws = getWindowStates();
			try {
				final FileOutputStream fos = new FileOutputStream(ws);
				final ObjectOutputStream oos = new ObjectOutputStream(fos);
				Window w = null;
				while ((w = Window.iterate(w)) != null)
					w.writeState(oos);
				oos.close();
				fos.close();
			} catch (IOException e) {
				Log.e(TAG, "error while writing windowstates", e);
			}
		}
    }
    
    private File getWindowStates() {
		return new File(mDataDir, "windowStates");
	}

	private File getBookmark() {
		return new File(mDataDir, "bookmark");
	}

	@Override
    protected void onDestroy() {
		super.onDestroy();
		glk.postExitEvent();
    }
    
    @Override
    protected void onSaveInstanceState(Bundle outState) {
    	super.onSaveInstanceState(outState);

    	if (!glk.isAlive())
    		return;

    	ArrayList<Parcelable> states = new ArrayList<Parcelable>();
    	
    	Window w = null;
    	while ((w = Window.iterate(w)) != null)
    		states.add(w.saveInstanceState());
    	
    	outState.putParcelableArrayList("windowStates", states);
    }
}
