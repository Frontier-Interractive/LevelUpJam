# LevelUpJam



## Prerequisites
- Unreal Engine 5.6
- Visual Studio 2022 (with C++ development tools)
- VS Code (optional)

## Setup & Build Instructions

### 1. Clone Repository
```bash
git clone <your-repo-url>
cd LevelUpJam
```

### 2. Generate Project Files
Right-click `LevelUpJam.uproject` → **"Generate Visual Studio project files"**

### 3. Build the Project

#### Option A: Using Unreal Editor (Easiest)
1. Double-click `LevelUpJam.uproject`
2. If prompted to build, click **"Yes"**
3. Unreal will compile automatically

#### Option B: Using Visual Studio
1. Open `LevelUpJam.sln`
2. Set configuration to **"Development Editor"**
3. Build → **Build Solution** (Ctrl+Shift+B)

## Project Structure
- `Source/LevelUpJam/Public/BoxCharacter.h` - Main player character with Enhanced Input
- `Source/LevelUpJam/Private/BoxCharacter.cpp` - Character implementation
- `Source/LevelUpJam/Obstacle.h` - Obstacle system base class
- `Content/` - Blueprint assets, Input Actions, and game content

## Controls
- **WASD** - Move character
- **Mouse** - Camera look
- **Spacebar** - Jump

