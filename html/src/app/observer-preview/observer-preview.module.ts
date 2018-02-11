import { NgModule } from '@angular/core';
import { ObserverPreviewComponent } from "./observer-preview.component";
import {FormsModule} from "@angular/forms";
import {CommonModule} from "@angular/common";

@NgModule({
    imports: [
        CommonModule,
        FormsModule,
    ],
    exports: [
        ObserverPreviewComponent
    ],
    declarations: [
        ObserverPreviewComponent
    ],
    providers: [
    ]
})
export class ObserverPreviewModule {

}