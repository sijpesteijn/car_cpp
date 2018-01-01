import {Component, EventEmitter, Input, Output} from "@angular/core";
import { ObserverGroup } from "../race.service";

@Component({
    selector: 'observers-group',
    template: require('./observers-group.html')
})
export class ObserversGroupComponent {
    @Input() group: ObserverGroup;
    @Output() onChange = new EventEmitter();

    private updateObserver($event: any) {
        this.onChange.emit(this.group);
    }
}