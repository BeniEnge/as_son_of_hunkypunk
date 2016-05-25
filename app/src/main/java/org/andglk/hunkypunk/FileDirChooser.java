package org.andglk.hunkypunk;


import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import org.andglk.babel.Babel;

import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Created by Orkan on 21.05.2016.
 */
public class FileDirChooser extends DialogFragment {
    private static final String PARENT_DIR = "..";
    private AlertDialog.Builder builder;
    private ListView list;

    private File currentPath;
    private FileSelectedListener fileListener;
    private DirSelectedListener dirListener;
    private String[] extension = null;


    public void setExtension(String... extension) {
        this.extension = new String[extension.length];
        for (int i = 0; i < extension.length; i++) {
            this.extension[i] = extension[i].toLowerCase();
        }
    }

    public interface FileSelectedListener {
        void fileSelected(File file);
    }

    public interface DirSelectedListener {
        void dirSelected(File[] files);
    }

    public FileDirChooser setFileListener(FileSelectedListener fileListener) {
        this.fileListener = fileListener;
        return this;
    }

    public FileDirChooser setDirListener(DirSelectedListener dirListener) {
        this.dirListener = dirListener;
        return this;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        list = new ListView(getActivity());
        builder = new AlertDialog.Builder(getActivity());
        Toast.makeText(getActivity(), "To select whole folder, go to folder and press OK", Toast.LENGTH_SHORT).show();

        list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int which, long id) {
                String fileChosen = (String) list.getItemAtPosition(which);
                File chosenFile = getChosenFile(fileChosen);
                if (chosenFile.isDirectory()) {
                    refresh(chosenFile);
                } else {
                    if (fileListener != null) {
                        if (!Arrays.asList(Paths.ifDirectory().list()).contains(chosenFile.getName())) {
                            fileListener.fileSelected(chosenFile);
                            dismiss();
                            Toast.makeText(getActivity(), "Added: " + chosenFile.getName(), Toast.LENGTH_SHORT).show();
                        } else
                            Toast.makeText(getActivity(), chosenFile.getName() + " already exists!", Toast.LENGTH_SHORT).show();
                    }
                }

            }
        });

        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                if (dirListener != null) {
                    ArrayList<File> files = new ArrayList<File>(Arrays.asList(getFiles()));
                    ArrayList<String> existingList = new ArrayList<String>(Arrays.asList(Paths.ifDirectory().list()));
                    String output = "Already existing:";

                    ArrayList<File> remove = new ArrayList<File>();
                    for (File f : files) {
                        if (existingList.contains(f.getName())) {
                            remove.add(f);
                            output += "\n" + f.getName();
                        }
                    }
                    files.removeAll(remove);

                    output += "\n\nAdded:";
                    for (File f : files) {
                        output += "\n" + f.getName();
                    }
                    File[] filesR = new File[files.size()];
                    for (int i = 0; i < filesR.length; i++) {
                        filesR[i] = files.get(i);
                    }
                    dirListener.dirSelected(filesR);

                    Toast.makeText(getActivity(), output, Toast.LENGTH_SHORT).show();
                }
            }
        });

        builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                Toast.makeText(getActivity(), "Cancel", Toast.LENGTH_SHORT).show();
            }
        });
        builder.setView(list);
        refresh(Environment.getExternalStorageDirectory());
        return builder.create();
    }

    private File[] getFiles() {
        File[] files = null;
        if (currentPath.exists()) {
            files = currentPath.listFiles(new FileFilter() {
                @Override
                public boolean accept(File file) {
                    if (!file.isDirectory()) {
                        for (int i = 0; i < extension.length; i++) {
                            if (file.getName().toLowerCase().endsWith(extension[i]))
                                return true;
                        }
                    }
                    return false;
                }
            });
        }
        return files;
    }

    private void refresh(File path) {
        this.currentPath = path;
        if (path.exists()) {
            File[] dirs = path.listFiles(new FileFilter() {
                @Override
                public boolean accept(File file) {
                    return (file.isDirectory() && file.canRead());
                }
            });
            File[] files = path.listFiles(new FileFilter() {
                @Override
                public boolean accept(File file) {
                    if (!file.isDirectory()) {
                        if (!file.canRead()) {
                            return false;
                        } else if (extension == null) {
                            return true;
                        } else {
                            for (int i = 0; i < extension.length; i++) {
                                if (file.getName().toLowerCase().endsWith(extension[i]))
                                    return true;
                            }
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
            });

            // convert to an array
            int i = 0;
            String[] fileList;
            if (path.getParentFile() == null) {
                fileList = new String[dirs.length + files.length];
            } else {
                fileList = new String[dirs.length + files.length + 1];
                fileList[i++] = PARENT_DIR;
            }
            Arrays.sort(dirs);
            Arrays.sort(files);
            for (File dir : dirs) {
                fileList[i++] = dir.getName();
            }
            for (File file : files) {
                fileList[i++] = file.getName();
            }

            // refresh the user interface
            builder.setTitle(currentPath.getPath());
            list.setAdapter(new ArrayAdapter(getActivity(),
                    android.R.layout.simple_list_item_1, fileList) {
                @Override
                public View getView(int pos, View view, ViewGroup parent) {
                    view = super.getView(pos, view, parent);
                    ((TextView) view).setSingleLine(true);
                    return view;
                }
            });
        }
    }

    /**
     * Convert a relative filename into an actual File object.
     */
    private File getChosenFile(String fileChosen) {
        if (fileChosen.equals(PARENT_DIR)) {
            if (currentPath.equals(Environment.getExternalStorageDirectory()))
                return Environment.getExternalStorageDirectory();
            return currentPath.getParentFile();
        } else {
            return new File(currentPath, fileChosen);
        }
    }

}
