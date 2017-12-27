import {Component, EventEmitter, Input, Output} from "@angular/core";

@Component({
    selector: 'observers-group',
    template: require('./observers-group.html')
})
export class ObserversGroupComponent {
    @Input() group: any;
    @Output() onChange = new EventEmitter();

    private updateObserver($event: any) {
        this.onChange.emit($event);
    }
}