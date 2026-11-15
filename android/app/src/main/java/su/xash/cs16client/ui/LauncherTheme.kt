package su.xash.cs16client.ui

import android.os.Build
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Shapes
import androidx.compose.material3.Typography
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.dynamicDarkColorScheme
import androidx.compose.material3.dynamicLightColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.ExperimentalTextApi
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontVariation
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import su.xash.cs16client.R

private val Blue = Color(0xFF2F6FED)
private val BlueContainer = Color(0xFFDCE5FF)
private val OnBlueContainer = Color(0xFF122251)
private val Lavender = Color(0xFF7568E8)
private val LavenderContainer = Color(0xFFE8E3FF)
private val OnLavenderContainer = Color(0xFF221C4E)

private val LightScheme = lightColorScheme(
    primary = Blue,
    onPrimary = Color.White,
    primaryContainer = BlueContainer,
    onPrimaryContainer = OnBlueContainer,
    secondary = Lavender,
    onSecondary = Color.White,
    secondaryContainer = LavenderContainer,
    onSecondaryContainer = OnLavenderContainer,
    tertiary = Color(0xFF0E7C86),
    onTertiary = Color.White,
    tertiaryContainer = Color(0xFFCDECEF),
    onTertiaryContainer = Color(0xFF002F33),
    background = Color(0xFFF6F8FE),
    onBackground = Color(0xFF191C22),
    surface = Color(0xFFF6F8FE),
    onSurface = Color(0xFF191C22),
    surfaceVariant = Color(0xFFE1E5F0),
    onSurfaceVariant = Color(0xFF454A57),
    surfaceContainerLowest = Color.White,
    surfaceContainerLow = Color(0xFFF0F3FB),
    surfaceContainer = Color(0xFFEAEEF8),
    surfaceContainerHigh = Color(0xFFE4E9F4),
    surfaceContainerHighest = Color(0xFFDEE3EF),
)

private val DarkScheme = darkColorScheme(
    primary = Color(0xFFAFC6FF),
    onPrimary = Color(0xFF10214E),
    primaryContainer = Color(0xFF31447A),
    onPrimaryContainer = Color(0xFFDCE5FF),
    secondary = Color(0xFFCFC6FF),
    onSecondary = Color(0xFF221C4E),
    secondaryContainer = Color(0xFF474279),
    onSecondaryContainer = Color(0xFFE8E3FF),
    tertiary = Color(0xFF8CD3D8),
    onTertiary = Color(0xFF00363A),
    tertiaryContainer = Color(0xFF004F54),
    onTertiaryContainer = Color(0xFFCDECEF),
    background = Color(0xFF0B1020),
    onBackground = Color(0xFFE3E6F0),
    surface = Color(0xFF0B1020),
    onSurface = Color(0xFFE3E6F0),
    surfaceVariant = Color(0xFF2A3040),
    onSurfaceVariant = Color(0xFFAEB4C4),
    surfaceContainerLowest = Color(0xFF080C18),
    surfaceContainerLow = Color(0xFF111626),
    surfaceContainer = Color(0xFF151B2E),
    surfaceContainerHigh = Color(0xFF1B2236),
    surfaceContainerHighest = Color(0xFF222A40),
)

@OptIn(ExperimentalTextApi::class)
private val RobotoFlex = FontFamily(
    Font(R.font.robotoflex, FontWeight.Normal, variationSettings = FontVariation.Settings(FontVariation.weight(400))),
    Font(R.font.robotoflex, FontWeight.Medium, variationSettings = FontVariation.Settings(FontVariation.weight(500))),
    Font(R.font.robotoflex, FontWeight.SemiBold, variationSettings = FontVariation.Settings(FontVariation.weight(600))),
    Font(R.font.robotoflex, FontWeight.Bold, variationSettings = FontVariation.Settings(FontVariation.weight(700))),
)

private val LauncherTypography = Typography(
    headlineMedium = TextStyle(
        fontFamily = RobotoFlex,
        fontWeight = FontWeight.Bold,
        fontSize = 24.sp,
        lineHeight = 30.sp,
        letterSpacing = (-0.2).sp,
    ),
    headlineSmall = TextStyle(
        fontFamily = RobotoFlex,
        fontWeight = FontWeight.Bold,
        fontSize = 20.sp,
        lineHeight = 26.sp,
    ),
    titleLarge = TextStyle(
        fontFamily = RobotoFlex,
        fontWeight = FontWeight.Bold,
        fontSize = 19.sp,
        lineHeight = 25.sp,
    ),
    titleMedium = TextStyle(
        fontFamily = RobotoFlex,
        fontWeight = FontWeight.SemiBold,
        fontSize = 16.sp,
        lineHeight = 22.sp,
    ),
    titleSmall = TextStyle(
        fontFamily = RobotoFlex,
        fontWeight = FontWeight.SemiBold,
        fontSize = 14.sp,
        lineHeight = 20.sp,
    ),
    bodyLarge = TextStyle(
        fontFamily = RobotoFlex,
        fontWeight = FontWeight.Normal,
        fontSize = 15.sp,
        lineHeight = 22.sp,
        letterSpacing = 0.2.sp,
    ),
    bodyMedium = TextStyle(
        fontFamily = RobotoFlex,
        fontWeight = FontWeight.Normal,
        fontSize = 14.sp,
        lineHeight = 20.sp,
        letterSpacing = 0.15.sp,
    ),
    bodySmall = TextStyle(
        fontFamily = RobotoFlex,
        fontWeight = FontWeight.Normal,
        fontSize = 12.sp,
        lineHeight = 16.sp,
        letterSpacing = 0.2.sp,
    ),
    labelLarge = TextStyle(
        fontFamily = RobotoFlex,
        fontWeight = FontWeight.SemiBold,
        fontSize = 14.sp,
        lineHeight = 20.sp,
        letterSpacing = 0.1.sp,
    ),
    labelMedium = TextStyle(
        fontFamily = RobotoFlex,
        fontWeight = FontWeight.SemiBold,
        fontSize = 12.sp,
        lineHeight = 16.sp,
        letterSpacing = 0.2.sp,
    ),
    labelSmall = TextStyle(
        fontFamily = RobotoFlex,
        fontWeight = FontWeight.Medium,
        fontSize = 11.sp,
        lineHeight = 14.sp,
        letterSpacing = 0.3.sp,
    ),
)

private val LauncherShapes = Shapes(
    extraSmall = RoundedCornerShape(10.dp),
    small = RoundedCornerShape(14.dp),
    medium = RoundedCornerShape(20.dp),
    large = RoundedCornerShape(26.dp),
    extraLarge = RoundedCornerShape(32.dp),
)

@Composable
fun LauncherTheme(content: @Composable () -> Unit) {
    val dark = isSystemInDarkTheme()
    val context = LocalContext.current
    val scheme = when {
        Build.VERSION.SDK_INT >= Build.VERSION_CODES.S && dark -> dynamicDarkColorScheme(context)
        Build.VERSION.SDK_INT >= Build.VERSION_CODES.S -> dynamicLightColorScheme(context)
        dark -> DarkScheme
        else -> LightScheme
    }
    MaterialTheme(
        colorScheme = scheme,
        typography = LauncherTypography,
        shapes = LauncherShapes,
        content = content,
    )
}
