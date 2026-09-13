## Installation

`parser` is a single portable executable. Put it in one folder and add that folder
to `PATH` — after that the command works from any directory.

### 1. Create a tools folder

```
C:\Tools
```

Any folder works, but keep it outside `Downloads` and outside the project itself,
so an update or a cleanup never removes it.

### 2. Copy the executable

Download `parser.exe` from the
[latest release](https://github.com/lconantl/parser/releases/latest)
and drop it into `C:\Tools`.

### 3. Add the folder to PATH

**Option A — PowerShell (no admin rights needed):**

```powershell
$old = [Environment]::GetEnvironmentVariable('Path', 'User')
[Environment]::SetEnvironmentVariable('Path', "$old;C:\Tools", 'User')
```

Run this **once**. Running it twice appends a duplicate entry.

**Option B — GUI:**

1. Press `Win + R`, type `sysdm.cpl`, press Enter.
2. Go to **Advanced** → **Environment Variables**.
3. Under **User variables**, select `Path` → **Edit** → **New**.
4. Enter `C:\Tools` → **OK** on every dialog.

### 4. Verify

Open a **new** terminal — already-running ones keep the old `PATH`:

```powershell
where parser
parser --help
```

### Updating

Replace `C:\Tools\parser.exe` with the new build. `PATH` stays as it is,
so no reconfiguration is needed. Close any terminal that is currently running
the tool, otherwise Windows locks the file and the copy fails.

### Uninstalling

Delete `C:\Tools\parser.exe`, then remove the folder from `PATH`:

```powershell
$old = [Environment]::GetEnvironmentVariable('Path', 'User')
$new = ($old -split ';' | Where-Object { $_ -and $_ -ne 'C:\Tools' }) -join ';'
[Environment]::SetEnvironmentVariable('Path', $new, 'User')
```

### Troubleshooting

| Problem                      | Fix                                                              |
|:-----------------------------|:-----------------------------------------------------------------|
| `parser: command not found`  | Open a new terminal; `PATH` is read at startup                   |
| `where parser` finds nothing | Check the folder is listed in **User variables**, not **System** |
| Two versions run at once     | An old copy is still on `PATH`; `where parser` lists every match |
| Garbled Cyrillic output      | Use Windows Terminal or run `chcp 65001`                         |