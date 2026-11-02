package org.lunarnx.app;

import android.os.Bundle;
import org.libsdl.app.SDLActivity;
import org.libsdl.app.PlatformUtils;
import org.libsdl.app.BorealisHandler;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;

public class LunarActivity extends SDLActivity {
    @Override protected String[] getLibraries() { return new String[]{"SDL2", "main"}; }

    @Override protected String[] getArguments() {
        return getIntent().getBooleanExtra("ui_smoke", false)
            ? new String[]{"--ui-smoke"} : new String[]{};
    }

    @Override protected void onCreate(Bundle state) {
        super.onCreate(state);
        PlatformUtils.borealisHandler = new BorealisHandler();
        try {
            String revision = getPackageManager().getPackageInfo(getPackageName(), 0).lastUpdateTime + "";
            File marker = new File(getFilesDir(), ".resources-version");
            String previous = marker.isFile() ? new String(java.nio.file.Files.readAllBytes(marker.toPath()), StandardCharsets.UTF_8) : "";
            if (!revision.equals(previous)) {
                copyAssets("resources", new File(getFilesDir(), "resources"));
                try (FileOutputStream stream = new FileOutputStream(marker)) {
                    stream.write(revision.getBytes(StandardCharsets.UTF_8));
                }
            }
        } catch (Exception error) {
            android.util.Log.e("LunarNX", "Could not prepare application resources", error);
            finish();
        }
    }

    private void copyAssets(String asset, File destination) throws IOException {
        String[] entries = getAssets().list(asset);
        if (entries != null && entries.length > 0) {
            if (!destination.isDirectory() && !destination.mkdirs())
                throw new IOException("Cannot create resource directory");
            for (String entry : entries) copyAssets(asset + "/" + entry, new File(destination, entry));
        } else {
            try (InputStream input = getAssets().open(asset);
                 FileOutputStream output = new FileOutputStream(destination)) {
                byte[] buffer = new byte[32768];
                int count;
                while ((count = input.read(buffer)) != -1) output.write(buffer, 0, count);
            }
        }
    }

    @Override protected void onDestroy() {
        super.onDestroy();
        // Borealis/SDL2 keep process-wide UI state; start fresh after a real exit.
        if (isFinishing()) System.exit(0);
    }
}
