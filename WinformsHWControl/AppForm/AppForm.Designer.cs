namespace Vision
{
    partial class AppForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            ButtonLeftStepper1 = new Button();
            ButtonRightStepper1 = new Button();
            ButtonRightStepper2 = new Button();
            ButtonLeftStepper2 = new Button();
            ButtonPlus = new Button();
            label1 = new Label();
            label2 = new Label();
            ButtonMinus = new Button();
            ResponseslistBox = new ListBox();
            label3 = new Label();
            label4 = new Label();
            label5 = new Label();
            SuspendLayout();
            // 
            // ButtonLeftStepper1
            // 
            ButtonLeftStepper1.Location = new Point(99, 204);
            ButtonLeftStepper1.Name = "ButtonLeftStepper1";
            ButtonLeftStepper1.Size = new Size(85, 23);
            ButtonLeftStepper1.TabIndex = 0;
            ButtonLeftStepper1.Text = "Left";
            ButtonLeftStepper1.UseVisualStyleBackColor = true;
            ButtonLeftStepper1.Click += ButtonLeftStepper1_Click;
            // 
            // ButtonRightStepper1
            // 
            ButtonRightStepper1.Location = new Point(246, 204);
            ButtonRightStepper1.Name = "ButtonRightStepper1";
            ButtonRightStepper1.Size = new Size(85, 23);
            ButtonRightStepper1.TabIndex = 1;
            ButtonRightStepper1.Text = "Right";
            ButtonRightStepper1.UseVisualStyleBackColor = true;
            ButtonRightStepper1.Click += ButtonRightStepper1_Click;
            // 
            // ButtonRightStepper2
            // 
            ButtonRightStepper2.Location = new Point(246, 250);
            ButtonRightStepper2.Name = "ButtonRightStepper2";
            ButtonRightStepper2.Size = new Size(85, 23);
            ButtonRightStepper2.TabIndex = 2;
            ButtonRightStepper2.Text = "Right";
            ButtonRightStepper2.UseVisualStyleBackColor = true;
            ButtonRightStepper2.Click += ButtonRightStepper2_Click;
            // 
            // ButtonLeftStepper2
            // 
            ButtonLeftStepper2.Location = new Point(99, 250);
            ButtonLeftStepper2.Name = "ButtonLeftStepper2";
            ButtonLeftStepper2.Size = new Size(85, 23);
            ButtonLeftStepper2.TabIndex = 3;
            ButtonLeftStepper2.Text = "Left";
            ButtonLeftStepper2.UseVisualStyleBackColor = true;
            ButtonLeftStepper2.Click += ButtonLeftStepper2_Click;
            // 
            // ButtonPlus
            // 
            ButtonPlus.Location = new Point(383, 218);
            ButtonPlus.Name = "ButtonPlus";
            ButtonPlus.Size = new Size(85, 23);
            ButtonPlus.TabIndex = 5;
            ButtonPlus.Text = "+";
            ButtonPlus.UseVisualStyleBackColor = true;
            ButtonPlus.Click += ButtonPlus_Click;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(26, 177);
            label1.Name = "label1";
            label1.Size = new Size(96, 15);
            label1.TabIndex = 6;
            label1.Text = "Steppers control:";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(383, 177);
            label2.Name = "label2";
            label2.Size = new Size(78, 15);
            label2.TabIndex = 7;
            label2.Text = "LED Intensity:";
            // 
            // ButtonMinus
            // 
            ButtonMinus.Location = new Point(383, 256);
            ButtonMinus.Name = "ButtonMinus";
            ButtonMinus.Size = new Size(85, 23);
            ButtonMinus.TabIndex = 8;
            ButtonMinus.Text = "-";
            ButtonMinus.UseVisualStyleBackColor = true;
            ButtonMinus.Click += ButtonMinus_Click;
            // 
            // ResponseslistBox
            // 
            ResponseslistBox.FormattingEnabled = true;
            ResponseslistBox.HorizontalExtent = 300;
            ResponseslistBox.ItemHeight = 15;
            ResponseslistBox.Location = new Point(533, 69);
            ResponseslistBox.Name = "ResponseslistBox";
            ResponseslistBox.ScrollAlwaysVisible = true;
            ResponseslistBox.Size = new Size(348, 364);
            ResponseslistBox.TabIndex = 9;
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Font = new Font("Segoe UI", 14F);
            label3.Location = new Point(532, 38);
            label3.Name = "label3";
            label3.Size = new Size(109, 25);
            label3.TabIndex = 10;
            label3.Text = "Activity log:";
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new Point(26, 208);
            label4.Name = "label4";
            label4.Size = new Size(59, 15);
            label4.TabIndex = 11;
            label4.Text = "Stepper 1:";
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Location = new Point(26, 254);
            label5.Name = "label5";
            label5.Size = new Size(59, 15);
            label5.TabIndex = 12;
            label5.Text = "Stepper 2:";
            // 
            // AppForm
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(893, 450);
            Controls.Add(label5);
            Controls.Add(label4);
            Controls.Add(label3);
            Controls.Add(ResponseslistBox);
            Controls.Add(ButtonMinus);
            Controls.Add(label2);
            Controls.Add(label1);
            Controls.Add(ButtonPlus);
            Controls.Add(ButtonLeftStepper2);
            Controls.Add(ButtonRightStepper2);
            Controls.Add(ButtonRightStepper1);
            Controls.Add(ButtonLeftStepper1);
            FormBorderStyle = FormBorderStyle.FixedSingle;
            Name = "AppForm";
            Text = "VisionForm";
            FormClosing += AppForm_FormClosing;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Button ButtonLeftStepper1;
        private Button ButtonRightStepper1;
        private Button ButtonRightStepper2;
        private Button ButtonLeftStepper2;
        private Button ButtonPlus;
        private Label label1;
        private Label label2;
        private Button ButtonMinus;
        private ListBox ResponseslistBox;
        private Label label3;
        private Label label4;
        private Label label5;
    }
}
