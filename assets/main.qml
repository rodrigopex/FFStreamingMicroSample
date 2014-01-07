/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.2
import bb.multimedia 1.0

Page {
    Container {
        layout: DockLayout {
        }
        Container {
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            topMargin: 20
            Label {
                horizontalAlignment: HorizontalAlignment.Center
                topMargin: 40
                // Localized text with the dynamic translation and locale updates support
                text: qsTr("OpenAL test ") + Retranslate.onLocaleOrLanguageChanged
                textStyle.base: SystemDefaults.TextStyles.BigText
            }
            Button {
                topMargin: 40
                text: "Record"
                onClicked: {
                    openAlCtrl.record()
                }
            }
            Button {
                topMargin: 40
                text: "Play"
                onClicked: {
                    openAlCtrl.play()
                }
            }
            Button {
                topMargin: 40
                text: "Hello World!"
                onClicked: {
                    openAlCtrl.hello()
                }
            }
        }
    }
}
