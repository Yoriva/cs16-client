package su.xash.cs16client

import android.content.ComponentName
import android.content.Intent
import android.content.SharedPreferences
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.res.stringResource
import su.xash.cs16client.ui.LauncherScreen
import su.xash.cs16client.ui.LauncherTheme

abstract class LauncherActivity : ComponentActivity() {

    protected abstract val gamedir: String
    protected abstract val titleRes: Int
    protected abstract val subtitleRes: Int

    private var enginePackage by mutableStateOf<String?>(null)
    private val prefs: SharedPreferences by lazy {
        getSharedPreferences(PREFS, MODE_PRIVATE)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()

        if (intent?.action == MOD_ACTION) {
            handleModIntent()
            return
        }

        enginePackage = findEngine()

        setContent {
            var argv by rememberSaveable {
                mutableStateOf(prefs.getString(KEY_ARGV, DEFAULT_ARGV) ?: DEFAULT_ARGV)
            }
            val blocked = hasUserDll(argv)

            LauncherTheme {
                LauncherScreen(
                    title = stringResource(titleRes),
                    subtitle = stringResource(subtitleRes),
                    gamedir = gamedir,
                    version = BuildConfig.VERSION_NAME,
                    argv = argv,
                    onArgvChange = { argv = it },
                    blocked = blocked,
                    enginePackage = enginePackage,
                    onLaunch = {
                        if (!hasUserDll(argv)) {
                            prefs.edit().putString(KEY_ARGV, argv.trim()).apply()
                            val engine = findEngine()
                            enginePackage = engine
                            if (engine != null) launch(engine, buildArgv(argv))
                        }
                    },
                    onGetEngine = ::openEngineDownload,
                )
            }
        }
    }

    override fun onResume() {
        super.onResume()
        enginePackage = findEngine()
    }

    private fun handleModIntent() {
        val engine = findEngine()
        if (engine == null) {
            startActivity(
                Intent(Intent.ACTION_VIEW, Uri.parse(ENGINE_URL))
                    .setFlags(Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK),
            )
        } else {
            launch(engine, buildArgv(DEFAULT_ARGV))
        }
        finish()
    }

    private fun launch(enginePackage: String, argv: String) {
        startActivity(
            Intent()
                .setComponent(ComponentName(enginePackage, "su.xash.engine.XashActivity"))
                .setFlags(Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK)
                .putExtra("gamedir", gamedir)
                .putExtra("gamelibdir", applicationInfo.nativeLibraryDir)
                .putExtra("argv", argv)
                .putExtra("package", packageName),
        )
        finish()
    }

    private fun buildArgv(userArgv: String): String {
        val trimmed = userArgv.trim()
        return if (trimmed.isEmpty()) DLL_ARG else "$trimmed $DLL_ARG"
    }

    private fun openEngineDownload() {
        startActivity(Intent(Intent.ACTION_VIEW, Uri.parse(ENGINE_URL)))
    }

    private fun findEngine(): String? = when {
        isInstalled(ENGINE_TEST) -> ENGINE_TEST
        isInstalled(ENGINE_STABLE) -> ENGINE_STABLE
        else -> null
    }

    private fun isInstalled(pkg: String): Boolean = try {
        packageManager.getPackageInfo(pkg, 0)
        true
    } catch (_: PackageManager.NameNotFoundException) {
        false
    }

    companion object {
        private const val PREFS = "cs16_launcher"
        private const val KEY_ARGV = "argv"
        private const val DEFAULT_ARGV = "-dev 2 -log"
        private const val DLL_ARG = "-dll @metamod"
        private const val MOD_ACTION = "su.xash.engine.MOD"
        private const val ENGINE_TEST = "su.xash.engine.test"
        private const val ENGINE_STABLE = "su.xash.engine"
        private const val ENGINE_URL =
            "https://github.com/FWGS/xash3d-fwgs/releases/tag/continuous"
    }
}

private fun hasUserDll(value: String): Boolean =
    value.trim().split(Regex("\\s+")).any { it == "-dll" || it.startsWith("-dll=") }
