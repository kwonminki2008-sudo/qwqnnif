# Qwqnnif - Unreal 5.7 C++ One-vs-One AI Duel

Unreal Engine 5.7 기준으로 만든 간단한 1:1 대결 게임의 C++ 초기 구조입니다. 플레이어 캐릭터, 적 캐릭터, 적 AI 컨트롤러, 체력 컴포넌트, 게임 모드가 분리되어 있어 이후 블루프린트/애니메이션/맵 작업을 얹기 쉽도록 구성했습니다.

## 구조

```text
Qwqnnif.uproject
Config/
  DefaultEngine.ini      # 기본 GameMode 설정
  DefaultGame.ini        # 프로젝트 정보
  DefaultInput.ini       # WASD, 마우스, 공격 입력
Source/
  Qwqnnif.Target.cs
  QwqnnifEditor.Target.cs
  Qwqnnif/
    Qwqnnif.Build.cs
    Qwqnnif.cpp
    Public/
      AI/DuelEnemyAIController.h
      Characters/DuelCharacterBase.h
      Characters/DuelEnemyCharacter.h
      Characters/DuelPlayerCharacter.h
      Combat/HealthComponent.h
      Game/DuelGameMode.h
    Private/
      AI/DuelEnemyAIController.cpp
      Characters/DuelCharacterBase.cpp
      Characters/DuelEnemyCharacter.cpp
      Characters/DuelPlayerCharacter.cpp
      Combat/HealthComponent.cpp
      Game/DuelGameMode.cpp
```

## 현재 구현된 흐름

- `ADuelGameMode`
  - 기본 플레이어 클래스를 `ADuelPlayerCharacter`로 설정합니다.
  - 맵에 적이 없으면 플레이어 앞쪽에 `ADuelEnemyCharacter`를 하나 생성합니다.
  - 플레이어 또는 적이 죽으면 화면 메시지로 승패를 표시합니다.
- `ADuelPlayerCharacter`
  - WASD 이동, 마우스 회전, 좌클릭/스페이스 공격 입력을 처리합니다.
  - SpringArm + Camera 기본 구성을 포함합니다.
- `ADuelEnemyCharacter`
  - `ADuelEnemyAIController`를 기본 AI 컨트롤러로 사용합니다.
- `ADuelEnemyAIController`
  - 플레이어를 탐지하면 추적합니다.
  - 공격 범위에 들어오면 이동을 멈추고 공격합니다.
- `UHealthComponent`
  - 데미지 수신, 체력 감소, 사망 이벤트를 담당합니다.

## 에디터에서 바로 테스트하기

1. `Qwqnnif.uproject`를 Unreal Engine 5.7로 엽니다.
2. C++ 프로젝트 파일을 생성하고 빌드합니다.
3. 프로젝트 폴더는 `Downloads`나 `OneDrive`가 아닌 `C:\UnrealProjects\Qwqnnif` 같은 로컬 폴더에 두는 것을 권장합니다.
4. 저장하지 말고 기본 `Entry` 맵에서 바로 Play를 눌러도 됩니다. 에디터 화면은 검게 보여도 Play를 누르면 게임 모드가 바닥, 조명, 적, 안내 문구를 자동 생성합니다.
5. 나중에 맵을 직접 저장하고 싶다면 `파일 > 새 레벨`로 새 레벨을 만든 뒤 `Content/Maps`에 저장하세요. `/Engine/Maps/Entry`를 그대로 다른 이름으로 저장하면 Unreal 내부 맵 참조 오류가 날 수 있습니다.
6. `ADuelPlayerCharacter`, `ADuelEnemyCharacter` 기반 블루프린트를 만들어 메시/애니메이션을 연결합니다.
7. GameMode의 EnemyClass를 적 블루프린트로 바꾸면 같은 C++ AI 로직을 유지한 채 외형만 교체할 수 있습니다.

> 현재 적 추적은 초보 테스트가 쉽도록 NavMesh 없이 플레이어 방향으로 직접 이동합니다. Play 후 `Duel prototype started...` 안내 문구가 뜨면 C++ 게임 모드가 정상 실행된 것입니다. 나중에 제대로 된 레벨을 만들면 `NavMeshBoundsVolume` 기반 AI 이동으로 바꿀 수 있습니다.
